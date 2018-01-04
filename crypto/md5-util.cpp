/****************************************************************
* MD5 Checksum utilities
****************************************************************/
#include "md5-util.hpp"

using namespace std;

namespace crypto {

// Computes  md5  sum  as  above,  but  taking  bytes from vector.
string md5( vector<char> const& bytes ) {
    // Not sure if it is ok  to  call operator[0] on an empty vec-
    // tor, so to be safe just check.
    char const* buf = bytes.size() ? &bytes[0] : nullptr;
    return md5( buf, bytes.size() );
}

// Computes md5 sum of bytes taken from c_str().
string md5( string const& s ) {
    return md5( s.c_str(), s.size() );
}

// Computes  md5  sum  of bytes raw byte buffer with given length
// (number of bytes). Note that len can be zero and it yield  the
// correct md5 sum for a zero length string.
string md5( char const* bytes, size_t len ) {

	md5::md5_t md5;
	md5.process( bytes, len );
	md5.finish();

	char str[MD5_STRING_SIZE]; // includes null zero

	md5.get_string( str );

    // Remove null zero
    return string( str, MD5_STRING_SIZE-1 );
}

} // crypto
