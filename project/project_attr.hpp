/****************************************************************
* Project Attributes
****************************************************************/
#pragma once

#include "types.hpp"

#include <experimental/filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

/****************************************************************
* ProjectAttr
****************************************************************/
// Attributes that we care about in Project files. These are  the
// ones  (and the only ones) that will be extracted when we parse
// the project files.

struct ProjectAttr {

    ProjectAttr( ProjectAttr&& )                  = default;
    ProjectAttr& operator=( ProjectAttr&& )       = default;

    ProjectAttr( ProjectAttr const& )             = delete;
    ProjectAttr& operator=( ProjectAttr const&  ) = delete;

    PathVec     cl_includes;
    PathVec     cl_compiles;
    PathVec     search_paths;
    fs::path    int_dir;
    fs::path    out_dir;
    std::string project_name;
    OptStr      target_name;
    OptStr      target_ext;
    std::string uuid;

};

std::ostream& operator<<( std::ostream&      out,
                          ProjectAttr const& p );

/****************************************************************
* Derived Quantities
****************************************************************/
auto tlog_name( ProjectAttr const& attr ) -> std::string;

} // namespace project
