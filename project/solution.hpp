/****************************************************************
* Solution
****************************************************************/
#pragma once

#include "project.hpp"
#include "types.hpp"

#include <experimental/filesystem>
#include <map>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

struct SolutionFile {

    SolutionFile( PathVec&& projects );

    static SolutionFile read( fs::path const& file );

    PathVec projects;
};

struct Solution {

    Solution( std::map<fs::path, Project>&& projects );

    static Solution read( fs::path const&  file,
                          std::string_view platform );

    static Solution read( fs::path const&  file,
                          fs::path const&  base,
                          std::string_view platform );

    std::map<fs::path, Project> projects;
};

std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s );

std::ostream& operator<<( std::ostream&   out,
                          Solution const& s );

} // namespace project
