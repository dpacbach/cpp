/****************************************************************
* IO related utilities
****************************************************************/
#include "io.hpp"
#include "macros.hpp"

#include <cstdio>
#include <fstream>
#include <regex>

using namespace std;

namespace util {

// Read  a  file in its entirety into a vector of chars. This may
// be a bit less efficient than possible because the vector, when
// created, will initialize all of its bytes  to  zero  which  we
// don't actually need.
vector<char> read_file( fs::path const& p ) {

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

// We should not need this function  because  the  filesystem  li-
// brary provides fs::copy_file which would ideally be better  to
// use. However, it was observed at  the  time  of  this  writine
// (with  gcc  7.2)  that  the fs::copy_file method was not faith-
// fully copying files  on  Windows  that  were  created on Linux.
// Specifically, it was observed that,  copying  a text file with
// Linux  line endings on Windows resulted in a new file with Win-
// dows line endings, which is  not  desired.  Hence we have this
// function which will copy the file in  binary  mode  faithfully.
void copy_file( fs::path const& from, fs::path const& to ) {
    // RVO  +  move semantics should ensure that the file data is
    // not unnecessarily copied.
    write_file( to, read_file( from ) );
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

// Take a path whose last  component  (file name) contains a glob
// expression and  return  results  by  searching  the  directory
// listing for all files (and folders if flag is true) that match
// the glob pattern. Only *  and  ?  are supported, and those spe-
// cial  characters  can only appear in the file name of the path.
// The filename (with wildcard characters)  must match the entire
// file name from start to finish. If one of the folders  in  the
// path  does  not  exist, an exception is thrown. If the path is
// relative then it is relative to CWD. Paths returned will  have
// their  absolute/relative  nature  preserved  according  to the
// input  p.  Also,  if input path is empty, it will return empty.
PathVec wildcard( fs::path const& p, bool with_folders ) {

    if( p.empty() )
        return {};

    // We need to preserve relative/absolute nature of input when
    // we  return  the  list  of output paths. Note that, just as
    // with the input, relative paths in  the output will be rela-
    // tive to CWD.
    bool rel = p.is_relative();

    PathVec res;

    auto abs    = util::lexically_absolute( p );
    auto folder = abs.parent_path();

    // Convert  the  glob  pattern to a regex so we can use regex
    // machinery  to  match  files. Note that here we are not sup-
    // porting the full set  of  shell-style  glob symbols. As an
    // example, the glob "*.?pp" will  be  transformed  into  the
    // regex: ".*\..pp"
    string rx_glob;
    for( auto c : abs.filename().string() ) {
        switch( c ) {
            case '.': rx_glob += "\\."; break;
            case '*': rx_glob += ".*";  break;
            case '?': rx_glob += '.';   break;
            default : rx_glob += c;     break;
        };
    }

    auto cwd = fs::current_path();

    // Regex  must  match  full filename, so we surround with ^$.
    regex rx( string( "^" ) + rx_glob + "$" );
    smatch m;

    for( auto& i : fs::recursive_directory_iterator( folder ) ) {
        if( fs::is_directory( i ) && !with_folders )
            // Don't include folders if  caller doesn't want them.
            continue;
        // This must be an lvalue for regex_match.
        auto fn = i.path().filename().string();
        if( regex_match( fn, m, rx ) ) {
            res.emplace_back(
                // Match,  add  it to the list. But we need to be
                // sure  to  preserve  absolute/relative   nature.
                rel ? util::lexically_relative( i.path(), cwd )
                    : fs::path( i )
            );
        }
    }
    return res;
}

} // util
