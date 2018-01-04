/****************************************************************
* MD5 Checksum utilities
****************************************************************/
#pragma once

#include "md5.h"

#include <string>
#include <vector>

namespace crypto {

// Computes  md5  sum  as  above,  but  taking  bytes from vector.
std::string md5( std::vector<char> const& bytes );

// Computes md5 sum of bytes taken from c_str().
std::string md5( std::string const& s );

// Computes  md5  sum  of bytes raw byte buffer with given length
// (number of bytes). Note that len can be zero and it yield  the
// correct md5 sum for a zero length string.
std::string md5( char const* bytes, size_t len );

} // namespace crypto
