// //////////////////////////////////////////////////////////
// HuffmanTree.h
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
//

#pragma once

#pragma warning (disable: 4530) // missing exception handling
#include <vector>
#include <cstddef>


/// Huffman tree
class HuffmanTree
{
public:
  /// store 16 bits (see MaxCodeLength)
  typedef unsigned short Code;
  /// code length in bits
  typedef unsigned char  NumBits;

  /// dictionary entry
  struct Leaf
  {
    // empty/invalid entry
    Leaf() : code(0), numBits(0) {}
    // valid entry
    Leaf(Code code_, NumBits bits_) : code(code_), numBits(bits_) {}

    // code bits
    Code    code;
    // length of code in bits
    NumBits numBits;
  };
  /// dictionary
  typedef std::vector<Leaf> Dictionary;

  /// bits per code
  typedef std::vector<NumBits> BitLengths;

  /// max. bits per code
  static const NumBits MaxCodeLength = 15;
  /// default max. bit length of instant lookup codes
  static const NumBits InstantMaxBit = 10;
  /// marks end of a block
  static const Code    EndOfBlock = 256;

  /// mapping direction
  enum Mapping { Encoding, Decoding };

  /// empty tree
  HuffmanTree() : _leaves(), _minBits(MaxCodeLength), _maxBits(0), _instantMaxBit(0) {}
  /// build tree
  explicit HuffmanTree(const BitLengths& bitLengthMapping, Mapping direction = Decoding);

  /// true, if encoding
  bool    isEncoding() const { return _isEncoding; }

  /// length of shortest code
  NumBits getMinBits() const { return _minBits; }
  /// length of longest  code
  NumBits getMaxBits() const { return _maxBits; }
  /// current threshold for smallLeaves/leaves
  NumBits getInstantMaxBit() const { return _instantMaxBit; }
  /// true, if empty
  bool    empty()      const { return _leaves.empty(); }

  /// look up a leaf
  Leaf getLeaf(size_t index) const { return _leaves[index]; }

  /// return Deflate's default bitlength tree
  static HuffmanTree getDefaultTree();

private:
  /// true, if encoding
  bool       _isEncoding;
  /// dictionary of all long codes/bits
  Dictionary _leaves;
  /// length of shortest code
  NumBits    _minBits;
  /// length of longest  code
  NumBits    _maxBits;
  /// current threshold for smallLeaves/leaves
  NumBits    _instantMaxBit;
};
