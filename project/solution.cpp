/****************************************************************
* Solution File Parsing and Processing
****************************************************************/
#include "fs.hpp"
#include "macros.hpp"
#include "solution.hpp"
#include "string-util.hpp"
#include "util.hpp"

#include <fstream>
#include <regex>

using namespace std;

namespace project {

/****************************************************************
* SolutionFile
****************************************************************/
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

// Reads in the solution file and parses it for  any  lines  that
// declare  projects, and then extracts the project file location
// (vcxproj) for each project.
SolutionFile SolutionFile::read( fs::path const&  file ) {

    ifstream in( file );
    ASSERT( in.good(), "failed to open solution file " << file );

    regex p_regex( project_line );
    smatch m;
    PathVec ps;

    for( string line; getline( in, line ); ) {
        // Strip the line to eliminate any windows  line  endings
        // which getline may leave  (though  it should remove the
        // linux line endings.
        line = util::strip( line );
        if( regex_match( line, m, p_regex ) ) {
            // size == 1 (total match) + 1 (subgroup)
            ASSERT_( m.size() == 2 );
            ps.push_back( util::fwd_slashes( string( m[1] ) ) );
        }
    }
    return SolutionFile( move( ps ) );
}

// For  debugging,  just  prints  out a list of the project paths
// (which  will be relative to solution folder) found in the solu-
// tion file.
std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s ) {
    util::print_vec( s.projects, out, true,
                     "Projects in Solution:" );
    return out;
}

/****************************************************************
* Solution
****************************************************************/
Solution::Solution( map<fs::path, Project>&& projects )
    : projects( move( projects ) )
{ }

// Read in a solution file, parse it  to get the list of projects
// that  it contains, then read in each project file and parse it
// completely.
Solution Solution::read( fs::path const& file,
                         string_view     plat,
                         fs::path const& base ) {

    SolutionFile sf = SolutionFile::read( file );

    auto abs_dir = util::absnormpath( file ).parent_path();

    map<fs::path, Project> m;

    for( auto const& path : sf.projects ) {
        auto abs = util::normpath( abs_dir / path );
        auto rel = base.empty()
                 ? abs : util::lexically_relative( abs, base );

        m.insert( pair( rel, Project::read( abs, base, plat ) ) );
    }

    return Solution( move( m ) );
}

// For debugging, just outputs the list of projects and, for each
// project,  tells  the  project to print itself, which typically
// results in a lot of output.
std::ostream& operator<<( std::ostream&   out,
                          Solution const& s ) {
    for( auto const& [path, project] : s.projects ) {
        out << path << ":" << endl << endl;
        out << project << endl;
    }
    return out;
}

} // namespace project
