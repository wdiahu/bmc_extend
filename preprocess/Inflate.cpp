// //////////////////////////////////////////////////////////
// Inflate.cpp
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
//

#include "Inflate.h"
#include "BinaryInputBuffer.h"
#include "Hash.h"

#include <memory.h>
#include <cassert>
#include <algorithm>
#include <iterator>

// some constants ...
static const HuffmanTree::Code MaxBitLength = 288;
static const HuffmanTree::Code MaxToken     = 285;
static const HuffmanTree::Code InvalidToken = 300;

// statics ...
HuffmanTree Inflate::_defaultTree = HuffmanTree::getDefaultTree();
HuffmanTree Inflate::_emptyTree;


/// decode a file
Inflate::Inflate(std::vector<std::string> &vecPattern)
: std::vector<unsigned char>(),
  _error(NotStarted),
  _filename(), // filename stored in gzip !
  _comment(),
  _timestamp(0),
  _compressionLevel(0),
  _sizeCompressed(0),
  _sizeParsed(0)
{
}

Inflate::Inflate()
	: std::vector<unsigned char>(),
	_error(NotStarted),
	_filename(), // filename stored in gzip !
	_comment(),
	_timestamp(0),
	_compressionLevel(0),
	_sizeCompressed(0),
	_sizeParsed(0)
{
}

int Inflate::DecodeBuffer(char *buf, size_t size, ByteInfoList *infoList)
{
	// open file
	BinaryInputBuffer buffer(buf, size);
	_sizeCompressed = buffer.size();
	if (_sizeCompressed == 0)
	{
		_error = InvalidFile;
		return 0;
	}
	// ... and decode data
	m_infoList = infoList;
	return load(buffer, false, true);
}

int Inflate::DecodeBuffer(std::string strFile, ByteInfoList *infoList)
{
	// open file
	BinaryInputBuffer buffer(strFile);
	_sizeCompressed = buffer.size();
	if (_sizeCompressed == 0)
	{
		_error = InvalidFile;
		return 0;
	}
	// ... and decode data
	m_infoList = infoList;
	return load(buffer, false, true);
}

/// main decoder
int Inflate::load(BinaryInputBuffer& buffer, bool isRawStream, bool checkCrc)
{
  _error = Working;
  
  // heuristic
  size_t uncompressedSize = buffer.getNumBitsLeft() / 5;

  // GZ header
  if (!isRawStream)
  {
    if (!parseHeader(buffer))
      return 0; // error codes are set by parseHeader

    // get uncompressed filesize
    uncompressedSize  = buffer.at(buffer.size()-1) << 24;
    uncompressedSize += buffer.at(buffer.size()-2) << 16;
    uncompressedSize += buffer.at(buffer.size()-3) <<  8;
    uncompressedSize += buffer.at(buffer.size()-4);
  }

  // update CRC32 on-the-fly
  unsigned int rollingCrc32 = 0; // intermediate hash
  size_t       checkedCrc32 = 0; // bytes processed

  // start decompression
  bool isFinal = false;
  while (decodeBlock(buffer, isFinal))
  {
    _sizeParsed = buffer.getNumBytesRead();

    // done with decoding ?
    if (isFinal)
      break;
  }

  // aborted during block decoding ?
  if (!isFinal)
  {
    _error = InvalidData;
    _sizeParsed = buffer.getNumBytesRead();
    return 0;
  }

  // skip fill bits
  buffer.removeBits(buffer.getNumBitsLeft() % 8);
  // check data integrity
  if (!isRawStream && checkCrc)
  {
    // calac CRC
	  rollingCrc32 = buffer.getInt();

    // expected size
	m_nUncompLen = buffer.getInt();
  }

  // great news ...
  _sizeParsed = buffer.getNumBytesRead();
  _error = Successful;
  return uncompressedSize;
}

/// parse header
bool Inflate::parseHeader(BinaryInputBuffer& buffer)
{
  // GZ signature
  unsigned char id1 = buffer.getByte();
  unsigned char id2 = buffer.getByte();
  assert(id1 == 0x1F && id2 == 0x8B);
  if (id1 != 0x1F || id2 != 0x8B)
  {
    _error = InvalidHeaderID;
    _sizeParsed = buffer.getNumBytesRead();
    return false;
  }

  // must be DEFLATE algorithm
  unsigned char algorithm = buffer.getByte();
  assert(algorithm == 0x08);
  if (algorithm != 0x08)
  {
    _error = InvalidCompressionAlgorithm;
    _sizeParsed = buffer.getNumBytesRead();
    return false;
  }

  // various flags
  unsigned char flags = buffer.getByte();

  // timestamp
  _timestamp = time_t(buffer.getInt());

  // compression level
  _compressionLevel = buffer.getByte();

  // skip file system
  buffer.getByte();
  //   0 - FAT (MS-DOS, OS/2, NT/Win32)
  //   1 - Amiga
  //   2 - VMS (or OpenVMS)
  //   3 - Unix
  //   4 - VM/CMS
  //   5 - Atari TOS
  //   6 - HPFS (OS/2, NT)
  //   7 - Macintosh
  //   8 - Z-System
  //   9 - CP/M
  //  10 - TOPS-20
  //  11 - NTFS (NT)
  //  12 - QDOS
  //  13 - Acorn RISCOS
  // 255 - unknown

  // extras (unused)
  if (flags & 0x04)
  {
    unsigned numBytes = buffer.getWord();
    while (numBytes--)
      buffer.getByte();
  }

  // filename
  if (flags & 0x08)
  {
    _filename.clear();
    char copy;
    while (0 != (copy = buffer.getByte()))
      _filename += copy;
  }

  // comment
  if (flags & 0x10)
  {
    char copy;
    while (0 != (copy = buffer.getByte()))
      _comment += copy;
  }

  // header CRC16 (unused)
  if (flags & 0x02)
    buffer.getWord();

  return true;
}

/// decode block (any method), true if okay
bool Inflate::decodeBlock (BinaryInputBuffer& buffer, bool& isFinal)
{
  // block header
  isFinal = buffer.getBool();
  size_t method = buffer.getBits(2);

  // raw block (uncompressed)
  if (method == 0)
    return copyBlock(buffer);

  // using static tree
  if (method == 1)
    return inflateBlock(buffer, _defaultTree, _emptyTree);

  // read dynamic tree
  if (method == 2)
  {
    // alphabet size
    size_t numLiterals   = 257 + buffer.getBits(5);
    size_t numDistance   =   1 + buffer.getBits(5);
    size_t numCodeLength =   4 + buffer.getBits(4);

    // read 19 codes defining bit lengths
    const size_t CodeLengths = 19;
    HuffmanTree::BitLengths codeLength(CodeLengths);
    static const unsigned char CodeLengthOrder[CodeLengths] =
    { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
    for (size_t i = 0; i < numCodeLength; i++)
      codeLength[CodeLengthOrder[i]] = (unsigned char)buffer.getBits(3);
    HuffmanTree codeStrings(codeLength, HuffmanTree::Decoding);

    // read bit lengths themselves
    HuffmanTree::Code lastToken = 0;
    HuffmanTree::BitLengths bitLength;
    bitLength.reserve(MaxBitLength);
    while (bitLength.size() < numLiterals+numDistance)
    {
      HuffmanTree::Code token = nextToken(buffer, codeStrings);
      unsigned int howOften = 0;

      // single literal
      if (token < 16)
      {
        howOften  = 1;
        lastToken = token;
      }
      // repeat last 3x-6x
      else if (token == 16)
      {
        howOften = 3 + buffer.getBits(2);
      }
      // 3x-10x zero
      else if (token == 17)
      {
        howOften  = 3 + buffer.getBits(3);
        lastToken = 0;
      }
      else if (token == 18)// 11x-138x zero
      {
        howOften  = 11 + buffer.getBits(7);
        lastToken = 0;
      }
      else
      {
        _error = InvalidData;
        _sizeParsed = buffer.getNumBytesRead();
        return false;
      }

      // repeat lastTaken (howOften times)
      while (howOften--)
        bitLength.push_back((unsigned char)lastToken);
    }

    // fill up with zeros
    bitLength.resize(numLiterals+32, 0);

    // extract distance code lengths
    HuffmanTree::BitLengths distanceLength;
    for (size_t i = 0; i < 32; i++)
      distanceLength.push_back(bitLength[i + numLiterals]);

    // cut back, only literals
    bitLength.resize(numLiterals);
    // removed too much ?
    bitLength.resize(MaxBitLength, 0);

    // let's decode !
    return inflateBlock(buffer, HuffmanTree(bitLength,      HuffmanTree::Decoding),
                                HuffmanTree(distanceLength, HuffmanTree::Decoding));
  }

  // invalid method
  _error = InvalidBlockMethod;
  _sizeParsed = buffer.getNumBytesRead();
  return false;
}

/// method 0
bool Inflate::copyBlock(BinaryInputBuffer& buffer)
{
  // skip all bits until reaching next byte boundary
  buffer.removeBits(buffer.getNumBitsLeft() % 8);

  // 16 bit block length
  size_t length     = buffer.getBits(16);
  // skip complementary block length
  size_t complement = buffer.getBits(16);

  if (length != 0xFFFF - complement)
    return false;

  // copy
  for (size_t i = 0; i < length; i++)
  {
	  unsigned char token = buffer.getByte();
// 	  push_back(token);
	  ProcessLiteral(token);
  }

  return true;
}

/// method 1&2
bool Inflate::inflateBlock(BinaryInputBuffer& buffer, const HuffmanTree& tree, const HuffmanTree& distanceTree)
{
  // decode tokens
  HuffmanTree::Code token;
  while (HuffmanTree::EndOfBlock != (token = nextToken(buffer, tree)))
  {
    // simple literal ?
    if (token < HuffmanTree::EndOfBlock)
    {
		ProcessLiteral(token);
		continue;
    }

    // error ?
    assert(token < InvalidToken);
    if (token == InvalidToken)
    {
      _error = InvalidData;
      _sizeParsed = buffer.getNumBytesRead();
      return false;
    }

    // it's a (length,distance) pair
    token -= 257;

    // get length, maybe with extra bits
    static const unsigned short CopyLength[29] =
    { /* 257 */   3, /* 258 */   4, /* 259 */   5,
      /* 260 */   6, /* 261 */   7, /* 262 */   8, /* 263 */   9, /* 264 */  10,
      /* 265 */  11, /* 266 */  13, /* 267 */  15, /* 268 */  17, /* 269 */  19,
      /* 270 */  23, /* 271 */  27, /* 272 */  31, /* 273 */  35, /* 274 */  43,
      /* 275 */  51, /* 276 */  59, /* 277 */  67, /* 278 */  83, /* 279 */  99,
      /* 280 */ 115, /* 281 */ 131, /* 282 */ 163, /* 283 */ 195, /* 284 */ 227,
      /* 285 */ 258 };
    size_t length = CopyLength[token];

    static const unsigned char ExtraLengthBits[29] =
    { /* 257 */ 0, /* 258 */ 0, /* 259 */ 0, /* 260 */ 0,
      /* 261 */ 0, /* 262 */ 0, /* 263 */ 0, /* 264 */ 0,
      /* 265 */ 1, /* 266 */ 1, /* 267 */ 1, /* 268 */ 1,
      /* 269 */ 2, /* 270 */ 2, /* 271 */ 2, /* 272 */ 2,
      /* 273 */ 3, /* 274 */ 3, /* 275 */ 3, /* 276 */ 3,
      /* 277 */ 4, /* 278 */ 4, /* 279 */ 4, /* 280 */ 4,
      /* 281 */ 5, /* 282 */ 5, /* 283 */ 5, /* 284 */ 5,
      /* 285 */ 0 };
    unsigned char extraLength = ExtraLengthBits[token];
    length += buffer.getBits(extraLength);

    // get distance
    HuffmanTree::Code distanceCode;
    if (distanceTree.empty())
    {
      distanceCode = HuffmanTree::Code(Bits::reverse(buffer.getBits(5), 5)); // fixed tree
    }
    else
    {
      distanceCode = nextToken(buffer, distanceTree); // dynamic tree
      if (distanceCode == InvalidToken)
      {
        _error = InvalidData;
        _sizeParsed = buffer.getNumBytesRead();
        return false;
      }
    }

    static const unsigned short CopyDistance[32] =
    {    1,     2,    3,    4,    5,    7,    9,    13,    17,    25,
        33,    49,   65,   97,  129,  193,  257,   385,   513,   769,
      1025,  1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577,
     32769, 49153 /* the last two are Deflate64 only */ };
    size_t distance = CopyDistance[distanceCode];

    static const unsigned char ExtraDistanceBits[32/2] =
    { 0,  0,  1,  2,  3, 4,  5,  6,  7,  8, 9, 10, 11, 12, 13, 14 /* the last is Deflate64 only */ };
    unsigned char moreBits = ExtraDistanceBits[distanceCode/2];
    distance += buffer.getBits(moreBits);

    // byte-wise copy
	ProcessDistCode(distance, length);
  }

  return true;
}

/// extract token from buffer, based on current Huffman tree
HuffmanTree::Code Inflate::nextToken(BinaryInputBuffer& buffer, const HuffmanTree& tree)
{
  // get bits
  HuffmanTree::Code compareTo = HuffmanTree::Code(buffer.peekBits(tree.getMaxBits()));

  // find matching code
  for (HuffmanTree::NumBits bits = tree.getInstantMaxBit(); bits <= tree.getMaxBits(); bits++)
  {
    // find leaf
    HuffmanTree::Code mask = (1 << bits) - 1;
    HuffmanTree::Leaf leaf = tree.getLeaf(compareTo & mask);

    // correct number of bits ?
    if (leaf.numBits <= bits)
    {
      // remove used bits
      buffer.removeBits(leaf.numBits);
      return leaf.code;
    }
  }

  // oops ?
  return InvalidToken;
}

void Inflate::WriteDecodeFile(std::string file)
{
}

void Inflate::ProcessDistCode(int dist, int length)
{
	memset(&m_byteInfo, 0, sizeof(m_byteInfo));

	m_byteInfo.d.dist = dist;
	m_byteInfo.d.length = length;
	m_infoList->push_back(m_byteInfo);

// 	int offset = m_infoList->size() - dist;
// 	for (int i = 0; i < length; i++)
// 	{
// 		push_back(operator[](offset + i));
// 	}
}

inline void Inflate::ProcessLiteral(char token)
{
	memset(&m_byteInfo, 0, sizeof(m_byteInfo));

	m_byteInfo.t.token = token;
// 	push_back(token);
	m_infoList->push_back(m_byteInfo);
}

/// error code translated to English (empty if Successful)
const char* Inflate::getErrorText() const
{
  switch (getError())
  {
  case Successful:
    return "";

  case NotStarted:
    return "Not Started Yet";

  case Working:
    return "Working";

  case InvalidPointer:
    return "Invalid Pointer";

  case InvalidFile:
    return "Invalid/Missing File";

  case InvalidHeaderID:
    return "Invalid Header ID (probably not a GZipped file)";

  case InvalidCompressionAlgorithm:
    return "Invalid Compression Algorithm (data corrupted)";

  case InvalidBlockMethod:
    return "Invalid Block Method (data corrupted)";

  case InvalidData:
    return "Invalid Data";

  case InvalidCrc:
    return "Invalid Crc (checksum error)";

  case FilesizeMismatch:
    return "Filesize Mismatch (number of decoded bytes differs from expected)";

  default:
    return "Unknown Error";
  }
}
