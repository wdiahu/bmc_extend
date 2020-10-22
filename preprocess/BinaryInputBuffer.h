// //////////////////////////////////////////////////////////
// BinaryInputBuffer.h
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
//

#pragma once

#pragma warning (disable: 4530) // missing exception handling
#include <string>
#include <fstream>


/// read file bit-wise
class BinaryInputBuffer
{
public:
  /// default buffer size if not completely loaded into memory
  enum { CacheSize = 256 };

  /// read from file (it's often faster to pre-load the complete file)
  explicit BinaryInputBuffer(const std::string& filename, bool preloadCompleteFile = true);
  /// already in memory
           BinaryInputBuffer(const void* buffer, size_t size);

  /// close file
  virtual ~BinaryInputBuffer();

  /// get number of bytes read so far
  size_t getNumBytesRead() const;
  /// get number of bits left in the buffer
  size_t getNumBitsLeft()  const;
  /// true, if no bits left
  bool   empty() const;
  /// total size in bytes, don't care about how much is read or left
  size_t size()  const;

  /// take a look at the next bits without modifying the buffer
  unsigned int peekBits  (unsigned char numBits);
  /// increment buffer pointers/offsets
  void         removeBits(unsigned char numBits);
  /// get the next bits and increment buffer pointers/offsets
  unsigned int   getBits (unsigned char numBits);
  /// get 8 bits
  unsigned char  getByte();
  /// get 16 bits
  unsigned short getWord();
  /// get 32 bits
  unsigned int   getInt ();
  /// get a single bit
  bool           getBool();

  /// get a single bit
  BinaryInputBuffer& operator>>(bool&           bit ) { bit   = getBool(); return *this; }
  /// get 8 bits
  BinaryInputBuffer& operator>>(unsigned char&  byte) { byte  = getByte(); return *this; }
  /// get 16 bits
  BinaryInputBuffer& operator>>(unsigned short& word) { word  = getWord(); return *this; }
  /// get 32 bits
  BinaryInputBuffer& operator>>(unsigned int&  dword) { dword = getInt (); return *this; }

  /// read byte at offset, and immediately go back to position we were before
  unsigned char at(size_t offset);

private:
  /// simple I/O buffering, faster than C lib's built-in functions
  unsigned char getBufferedByte();

  /// bit/byte file stream
  std::ifstream _stream;
  /// total number of bytes in the stream
  size_t   _size;
  /// total number of bytes read so far
  size_t   _read;
  /// total bits left (initially, it's 8*_size)
  size_t   _bitsLeft;
  /// store bits until next byte boundary
  unsigned int  _bitBuffer;
  /// number of valid bits in _bitBuffer
  unsigned char _bitBufferSize;

  /// keep in memory ?
  bool     _inMemory;
  /// new/delete allowed ?
  bool     _hasCacheOwnership;
  /// buffer
  unsigned char* _cache;
  /// position of next byte
  size_t _cacheOffset;
  /// position beyond last valid byte
  size_t _cacheSize;
};
