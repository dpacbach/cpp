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

Solution::Solution( map<fs::path, Project>&& projects )
    : projects( move( projects ) )
{ }

Solution Solution::read( fs::path const& file,
                         string_view     platform ) {
    return Solution::read( file, "", platform );
}

Solution Solution::read( fs::path const& file,
                         fs::path const& base,
                         string_view     platform ) {

    SolutionFile sf = SolutionFile::read( file );

    auto abs_dir = util::absnormpath( file ).parent_path();

    auto use_rel = !base.empty();

    auto abs = [&]( auto const& p ) {
        return util::normpath( abs_dir / p );
    };

    auto abs_rel = [&]( auto const& p ) {
        auto res = abs( p );
        if( use_rel )
            res = util::lexically_relative( res, base );
        return res;
    };

    map<fs::path, Project> m;

    for( auto const& path : sf.projects ) {
        Project project = Project::read(
                              abs( path ), base, platform );
        m.emplace( make_pair(
                    abs_rel( path ), move( project ) ) );
    }

    return Solution( move( m ) );
}

std::ostream& operator<<( std::ostream&   out,
                          Solution const& s ) {
    for( auto const& [path, project] : s.projects ) {
        out << path << ":" << endl << endl;
        out << project << endl;
    }
    return out;
}

} // namespace project
