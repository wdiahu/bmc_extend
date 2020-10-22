// //////////////////////////////////////////////////////////
// BinaryInputBuffer.cpp
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
//

#include "BinaryInputBuffer.h"

#pragma warning (disable: 4996) // fopen is unsafe
#include <cassert>


/// read from file
BinaryInputBuffer::BinaryInputBuffer(const std::string& filename, bool preloadCompleteFile)
: _stream(),
  _size(0),
  _read(0),
  _bitsLeft(0),
  _bitBuffer(0),
  _bitBufferSize(0),
  _inMemory(preloadCompleteFile),
  _hasCacheOwnership(true),
  _cache(NULL),
  _cacheOffset(0),
  _cacheSize(0)
{
  assert(!filename.empty());
  if (filename.empty())
    return;

  _stream.open(filename.c_str(), std::ios::in | std::ios::binary);

  // open file
  if (!_stream)
    return;

  // get file size
  _stream.seekg(0, std::ios_base::end);
  _size = _stream.tellg();
  _stream.seekg(0, std::ios_base::beg);

  // load first segment (or even the complete file)
  _cacheSize = _inMemory ? _size : size_t(CacheSize);
  _cache = new unsigned char[_cacheSize];
  _stream.read((char*)_cache, _cacheSize);
  if (_inMemory)
    _stream.close();

  // total number of bits
  _bitsLeft = 8 * _size;
}


/// already in memory
BinaryInputBuffer::BinaryInputBuffer(const void* buffer, size_t size)
: _stream(),
  _size(size),
  _read(0),
  _bitsLeft(8 * size),
  _bitBuffer(0),
  _bitBufferSize(0),
  _inMemory(true),
  _hasCacheOwnership(false),
  _cache((unsigned char*)buffer),
  _cacheOffset(0),
  _cacheSize(size)
{
  assert(buffer);
  if (!buffer)
    _size = _bitsLeft = _cacheSize = 0;
}


/// close file
BinaryInputBuffer::~BinaryInputBuffer()
{
  if (_hasCacheOwnership)
    delete[] _cache;
}


/// get number of bytes read so far
size_t BinaryInputBuffer::getNumBytesRead() const
{
  return _read;
}


/// get number of bits left in the buffer
size_t BinaryInputBuffer::getNumBitsLeft() const
{
  return _bitsLeft;
}


/// true, if no bits left
bool BinaryInputBuffer::empty() const
{
  return _bitsLeft == 0;
}


/// total size in bytes, don't care about how much is read or left
size_t BinaryInputBuffer::size() const
{
  return _size;
}


/// take a look at the next bits without modifying the buffer
unsigned int BinaryInputBuffer::peekBits(unsigned char numBits)
{
  assert(numBits <= 16);

  // move 8 bits from stream to buffer
  while (_bitBufferSize < numBits)
  {
    // get one byte from stream and add to internal buffer
    unsigned int byte = getBufferedByte();
    _bitBuffer       |= byte << _bitBufferSize;
    _bitBufferSize   += 8;
  }

  // return desired bits
  unsigned int bitMask = (1 << numBits) - 1;
  return _bitBuffer & bitMask;
}


/// get the next bits and increment buffer pointers/offsets
unsigned int BinaryInputBuffer::getBits(unsigned char numBits)
{
  unsigned int result = peekBits(numBits);
  removeBits(numBits);
  return result;
}


/// get 8 bits
unsigned char BinaryInputBuffer::getByte()
{
  return (unsigned char)getBits(8);
}


/// get 16 bits
unsigned short BinaryInputBuffer::getWord()
{
  unsigned short firstByte = getByte();
  return firstByte | (getByte() << 8);
}


/// get 32 bits
unsigned int BinaryInputBuffer::getInt()
{
  assert(sizeof(unsigned int) == 4);
  unsigned int result = getByte();
  result |= getByte() <<  8;
  result |= getByte() << 16;
  result |= getByte() << 24;
  return result;
}


/// get a single bit
bool BinaryInputBuffer::getBool()
{
  return getBits(1) == 1;
}


/// increment buffer pointers/offsets
void BinaryInputBuffer::removeBits(unsigned char numBits)
{
  _bitBuffer    >>= numBits;
  _bitBufferSize -= numBits;
  _bitsLeft      -= numBits;
}


/// read byte at offset, and immediately go back to position we were before
unsigned char BinaryInputBuffer::at(size_t offset)
{
  // file already in memory ?
  if (_inMemory)
    return _cache[offset];

  // current position
  std::streampos current = _stream.tellg();

  // read byte
  char result;
  _stream.seekg(offset, std::ios_base::beg);
  _stream.get(result);
  // go back
  _stream.seekg(current, std::ios_base::beg);

  return (unsigned char)result;
}


/// simple I/O buffering, faster than C lib's built-in functions
unsigned char BinaryInputBuffer::getBufferedByte()
{
  // fill buffer
  if (_cacheOffset >= _cacheSize)
  {
    assert(_stream);
    assert(!_inMemory);
    _stream.read((char*)_cache, CacheSize);
    _cacheOffset = 0;
  }

  _read++;

  // single byte
  return _cache[_cacheOffset++];
}
