/****************************************************************
* Solution
****************************************************************/
#include "fs.hpp"
#include "macros.hpp"
#include "solution.hpp"
#include "util.hpp"

#include <fstream>
#include <iostream>
#include <regex>

using namespace std;

namespace project {

// This  attempts to verify/collect just enough info to determine
// if a single line consists of a project statement  and,  if  so,
// extract the path of the project  file  from within it. It will
// look something like this:
//
// Project("...") = "...", "path\to\my_proj.vcxproj", ...
//
// In this case,  the  regex  captures  "path\to\my_proj.vcxproj",
// excluding the surrounding quotes.
auto project_line = "^ *Project[^,]+, *\"([^\"]+)\".*";

SolutionFile::SolutionFile( PathVec&& projects )
    : projects( move( projects ) )
{ }

SolutionFile SolutionFile::read( fs::path const&  file ) {

    ifstream in( file );
    ASSERT( in.good(), "failed to open solution file " << file );

    regex p_regex( project_line );
    smatch m;
    PathVec ps;

    for( string line; getline( in, line ); ) {
        if( regex_match( line, m, p_regex ) ) {
            // size == 1 (total match) + 1 (subgroup)
            ASSERT_( m.size() == 2 );
            ps.push_back( util::fwd_slashes( string( m[1] ) ) );
        }
    }
    return SolutionFile( move( ps ) );
}

std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s ) {

    util::print_vec( s.projects, out, true,
                     "Projects in Solution:" );

    return out;
}

} // namespace project
