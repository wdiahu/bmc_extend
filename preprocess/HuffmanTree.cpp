// //////////////////////////////////////////////////////////
// HuffmanTree.cpp
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
//

#include "HuffmanTree.h"
#include "Hash.h" // Bits::reverse
#include <cassert>
#include <algorithm> // std::min / std::max

/// create new Huffman tree based on bit code lengths
HuffmanTree::HuffmanTree(const BitLengths& bitLengthMapping, Mapping direction)
: _isEncoding(direction == Encoding),
  _leaves(),
  _minBits(MaxCodeLength),
  _maxBits(0),
  _instantMaxBit(0)
{
  // get frequencies of each bit length and ignore 0's
  Code bitLengthCount[MaxCodeLength+1] = {0};
  for (size_t i = 0; i < bitLengthMapping.size(); i++)
    bitLengthCount[bitLengthMapping[i]]++;
  bitLengthCount[0] = 0;

  // shortest and longest codes
  _maxBits = 0;
  _minBits = MaxCodeLength;
  for (NumBits bits = 1; bits <= MaxCodeLength; bits++)
  {
    if (bitLengthCount[bits] == 0)
      continue;

    _minBits = std::min(_minBits, bits);
    _maxBits = std::max(_maxBits, bits);
  }
  _leaves.resize(1 << _maxBits);

  // smallest suitable cache
  _instantMaxBit = std::min(InstantMaxBit, _maxBits);
  Code instantMask = (1 << _instantMaxBit) - 1;

  // find bit code for first element of each bitLength group
  Code code = 0;
  Code nextCode[MaxCodeLength + 1] = { 0 };
  for (NumBits bits = _minBits; bits <= _maxBits; bits++)
  {
    code  += bitLengthCount[bits-1];
    code <<= 1;
    nextCode[bits] = code;
  }

  // create binary codes for each literal
  for (Code i = 0; i < bitLengthMapping.size(); i++)
  {
    NumBits bits = bitLengthMapping[i];

    // unused literal ?
    if (bits == 0)
      continue;

    // get canonical code
    Code canonical = nextCode[bits];
    nextCode[bits]++;

    /// reverse bit order
    assert(bits <= MaxCodeLength);
    Code reverse = Code(Bits::reverse((unsigned int)canonical, bits));

    // store in encoding dictionary
    if (_isEncoding)
    {
      _leaves[i] = Leaf(reverse, bits);
      continue;
    }

    // store in decoding dictionary
    Leaf leaf = Leaf(i, bits);
    _leaves[reverse] = leaf;

    // replicate short codes ?
    if (bits <= _instantMaxBit)
    {
      // spread to all longer index positions
      Code step = 1 << bits;
      for (Code spread = reverse+step; spread <= instantMask; spread += step)
        _leaves[spread] = leaf;
    }
  }
}


/// return Deflate's default bitlength tree
HuffmanTree HuffmanTree::getDefaultTree()
{
  HuffmanTree::BitLengths defaultBitLengths(288, 0);

  size_t i = 0;
  for (; i < 144; i++) defaultBitLengths[i] = 8;
  for (; i < 256; i++) defaultBitLengths[i] = 9;
  for (; i < 280; i++) defaultBitLengths[i] = 7;
  for (; i < 288; i++) defaultBitLengths[i] = 8;

  return HuffmanTree(defaultBitLengths, HuffmanTree::Decoding);
}
