/****************************************************************
* Mock Preprocessor
****************************************************************/
#include "preprocessor.hpp"
#include "macros.hpp"
#include "string-util.hpp"

#include <algorithm>
#include <fstream>
#include <regex>

using namespace std;

namespace project {

namespace {

// This  is a regex to scan for a #include preprocessor statement.
// Note: it allows mismatching quotes  or brackets, i.e., the fol-
// lowing is valid: #include  "a/b/c>.  Oh  well. Also, hopefully
// this will be compiled once (as  a  global variable) so that it
// is faster to use multiple times.
regex re_inc( "^[ \t]*#[ \t]*include[ \t]*[\"<]([^\"> ]*)[\">].*" );

// These are a list of extensions marking files that are relevant
// to this preprocessor.
vector<fs::path> const exts{
    ".cpp", ".hpp", ".c", ".h", ".inl", ".cu", ".cuh"
};

// Returns true if p has extension in the above list (with
// case-insensitive comparison).
bool is_interesting( fs::path const& p ) {

    auto ext = p.extension();
    util::CaseSensitive sens = util::CaseSensitive::NO;

    auto pred = [&]( fs::path const& a ) {
        return util::path_equals( ext, a, sens );
    };

    return find_if( begin( exts ), end( exts ), pred )
           != end( exts );
}

}

// This method will, in a quite flexible way, scan for an include
// statement in the string and, if a  valid one is found, it will
// return the contents of the quotes or brackets.
OptPath parse_include( string const& sv ) {
    smatch m;
    if( !regex_match( sv, m, re_inc ) )
        return nullopt;
    ASSERT_( m.size() == 2 );
    return string( m[1] );
}

// Open a file and parse every line for an include statement  and
// return a list of the inner contents (file/path).
PathVec parse_includes( fs::path const& file ) {

    ifstream in( file.string() );
    ASSERT( in.good(), "failed to open file " << file );

    PathVec res;

    for( string line; getline( in, line ); ) {
        // Strip the line to eliminate any windows  line  endings
        // which getline may leave  (though  it should remove the
        // linux line endings.
        line = util::strip( line );
        auto path = parse_include( line );
        if( !path )
            continue;
        res.push_back( *path );
    }
    return res; // hoping for NRVO here
}

// Find  all source files under the `from` path, open them up and
// parse all the include statements  in  them,  then return a map
// where key is the file path relative to base_path and the value
// is a list of parsed (but not resolved) include files.
GlobalIncludeMap build_sources( fs::path from,
                                fs::path base_path ) {
    ASSERT_( from.is_absolute() && base_path.is_absolute() );
    GlobalIncludeMap res;
    for( auto& i : fs::recursive_directory_iterator( from ) ) {
        if( is_interesting( i ) ) {
            auto rel = util::lexically_relative( i, base_path );
            // Use i here since we don't want to assume that  the
            // CWD is equal to base_path.
            res[rel] = parse_includes( i );
        }
    }
    return res;
}

} // namespace project
