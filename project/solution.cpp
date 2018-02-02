/****************************************************************
* Solution File Parsing and Processing
****************************************************************/
#include "algo-par.hpp"
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

SolutionFile::SolutionFile( PathVec&& ps )
    : m_projects( move( ps ) )
{ }

// Reads in the solution file and parses it for  any  lines  that
// declare  projects, and then extracts the project file location
// (vcxproj) for each project.
SolutionFile SolutionFile::read( istream& in ) {

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

// This one takes file path.
SolutionFile SolutionFile::read( fs::path const& file ) {

    ifstream in( file.string() );
    ASSERT( in.good(), "failed to open solution file " << file );
    return SolutionFile::read( in );
}

// For  debugging,  just  prints  out a list of the project paths
// (which  will be relative to solution folder) found in the solu-
// tion file.
std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s ) {
    util::print_vec( s.projects(), out, true,
                     "Projects in Solution:" );
    return out;
}

/****************************************************************
* Solution
****************************************************************/
Solution::Solution( map<fs::path, Project>&& ps )
    : m_projects( move( ps ) )
{ }

// Read in a solution file, parse it  to get the list of projects
// that  it contains, then read in each project file and parse it
// completely.  Alhough  we're being given the contents of the so-
// lution file in a stream we still need to know the location  of
// the  solution  file's containing folder so that we can resolve
// the paths of the  project  files  referred  to in the solution
// file (which are  relative  to  the  location  of  the solution
// file).
Solution Solution::read( istream&        in,
                         fs::path const& sln_parent,
                         string_view     plat,
                         fs::path const& base ) {

    SolutionFile sf = SolutionFile::read( in );

    auto abs_dir = util::lexically_absolute( sln_parent );

    map<fs::path, Project> m;

    for( auto const& path : sf.projects() ) {
        auto abs = util::lexically_normal( abs_dir / path );
        auto rel = base.empty()
                 ? abs : util::lexically_relative( abs, base );

        m.insert( pair( rel, Project::read( abs, plat, base ) ) );
    }

    return Solution( move( m ) );
}

// This one takes a file path.
Solution Solution::read( fs::path const& file,
                         string_view     plat,
                         fs::path const& base ) {
    ifstream in( file.string() );
    ASSERT( in.good(), "failed to open solution file " << file );
    return Solution::read( in, file.parent_path(), plat, base );
}

// For debugging, just outputs the list of projects and, for each
// project,  tells  the  project to print itself, which typically
// results in a lot of output.
std::ostream& operator<<( std::ostream&   out,
                          Solution const& s ) {
    for( auto const& [path, project] : s.projects() ) {
        out << path << ":" << "\n\n";
        out << project << "\n";
    }
    return out;
}

} // namespace project
