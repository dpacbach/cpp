/****************************************************************
* Solution
****************************************************************/
#include "solution.hpp"

using namespace std;

namespace project {

SolutionFile::SolutionFile( PathVec&& projects )
    : projects( move( projects ) )
{ }

SolutionFile read( fs::path const&  file ) {

    PathVec ps;
    return SolutionFile( move( ps ) );
}

SolutionFile read( fs::path const&  file,
                   fs::path const&  base ) {

    PathVec ps;
    return SolutionFile( move( ps ) );
}

std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s ) {
    out << "Projects:" << endl;

    for( auto const& p : s.projects )
        out << "  " << p << endl;

    return out;
}

} // namespace project
