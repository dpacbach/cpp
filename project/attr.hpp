/****************************************************************
* Project Attributes
****************************************************************/
#pragma once

#include "types.hpp"
#include "non-copyable.hpp"

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

struct ProjectAttr : public util::non_copyable {

    PathVec     cl_includes;
    PathVec     cl_compiles;
    PathVec     search_paths;
    fs::path    int_dir;
    fs::path    out_dir;
    std::string project_name;
    OptStr      target_stem;
    OptStr      target_ext;
    std::string uuid;

};

std::ostream& operator<<( std::ostream&      out,
                          ProjectAttr const& p );

/****************************************************************
* Derived Quantities
****************************************************************/
auto tlog_name   ( ProjectAttr const& attr ) -> fs::path;
auto trg_name    ( ProjectAttr const& attr ) -> OptPath;
auto lib_name    ( ProjectAttr const& attr ) -> OptPath;
auto pdb_name    ( ProjectAttr const& attr ) -> OptPath;
auto exp_name    ( ProjectAttr const& attr ) -> OptPath;
auto lbs_name    ( ProjectAttr const& attr ) -> fs::path;
auto ubs_name    ( ProjectAttr const& attr ) -> fs::path;
auto src_folders ( ProjectAttr const& attr ) -> PathVec;

} // namespace project
