/****************************************************************
* Solution
****************************************************************/
#pragma once

#include "types.hpp"

#include <experimental/filesystem>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

struct SolutionFile {

    SolutionFile( PathVec&& projects );

    static SolutionFile read( fs::path const& file );

    static SolutionFile read( fs::path const& file,
                              fs::path const& base );

    PathVec projects;
};

std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s );

} // namespace project
