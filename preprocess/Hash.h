// //////////////////////////////////////////////////////////
// Hash.h
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
//

#pragma once

#include <cstddef>

namespace Hash
{
  /// compute CRC32 based on polynom 0xEDB88320L
  unsigned int crc32  (const void* data, size_t length, unsigned int previousCrc32   = 0);
}

namespace Bits
{
  /// reverse bit order
  unsigned int reverse(unsigned int x, size_t numBits);
}
