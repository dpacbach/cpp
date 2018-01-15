/****************************************************************
* IO related utilities
****************************************************************/
#include "io.hpp"
#include "macros.hpp"

#include <cstdio>
#include <fstream>

using namespace std;

namespace util {

// Read  a  file in its entirety into a vector of chars. This may
// be a bit less efficient than possible because the vector, when
// created, will initialize all of its bytes  to  zero  which  we
// don't actually need.
vector<char> read_file( fs::path p ) {

    ASSERT( fs::exists( p ), "file " << p << " does not exist" );

    size_t size = fs::file_size( p );
    vector<char> res( size );

    FILE* fp = fopen( p.string().c_str(), "rb" );
    ASSERT( fp, "failed to open file " << p );

    // Read the bytes.
    size_t read = fread( (void*)&res[0], 1, size, fp );
    // Close the file before checking  for  errors  (which  might
    // throw an exception).
    fclose( fp );

    // Read the bytes and be sure that we have  read  all  of  it.
    ASSERT( read == size, "failed to read all " << size <<
                          " bytes of file " << p << ".  Instead "
                          " read " << read << " bytes." );

    return res;
}

// Open the file, truncate it,  and  write  given  vector  to  it.
void write_file( fs::path const& p, vector<char> const& v ) {

    FILE* fp = fopen( p.string().c_str(), "wb" );
    ASSERT( fp, "failed to open or create file " << p );

    size_t size = v.size();

    size_t written = fwrite( (void*)&v[0], 1, size, fp );
    // Close the file before checking  for  errors  (which  might
    // throw an exception).
    fclose( fp );

    ASSERT( written == size, "failed to write all " << size <<
                             " bytes of vector to file " << p );
}

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
