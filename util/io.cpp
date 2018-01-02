/****************************************************************
* IO related utilities
****************************************************************/
#include "io.hpp"
#include "macros.hpp"

#include <fstream>

using namespace std;

namespace util {

// Read a text file into a string in its entirety.
string read_file_str( fs::path p ) {

    ifstream in( p.string() );
    ASSERT( in.good(), "failed to open file " << p );

    // Try to avoid having to resize/reallocate the string by  re-
    // serving enough space.
    auto size = fs::file_size( p );

    string res; res.reserve( size );

    bool first = true; // used to know whether to put a newline.

    for( string line; getline( in, line ); ) {
        if( !first) res += "\n";
        res += line;
        first = false;
    }

    // Verify that our calculation was correct; if not,  then  we
    // could pay  with  unnecessary  allocations.  We  allow  the
    // string to potentially be  one  character  smaller than the
    // computed size because the getline() function it seems will
    // ignore a newline character in the  file  if it is the very
    // last character.
    ASSERT( res.size() == size || res.size() == (size-1),
            "estimate of string size incorrect; res.size()"
            " == " << res.size() << ", size == " << size );

    return res; // hoping for NRVO here
}

// Read  a text file into a string in its entirety and then split
// it into lines.
StrVec read_file_lines( fs::path p ) {

    ifstream in( p.string() );
    ASSERT( in.good(), "failed to open file " << p );

    StrVec res;

    for( string line; getline( in, line ); )
        res.push_back( line );

    return res; // hoping for NRVO here
}

} // util
