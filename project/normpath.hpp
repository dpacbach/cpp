/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#pragma once

#include "fs.hpp"
#include "project.hpp"

namespace project {

struct ProjectNorm : Project {

    ProjectNorm( std::vector<fs::path>&&      cl_includes,
                 std::vector<fs::path>&&      cl_compiles,
                 std::vector<fs::path>&&      search_paths,
                 fs::path&&                   int_dir,
                 fs::path&&                   out_dir,
                 std::string&&                project_name,
                 std::optional<std::string>&& target_name,
                 std::optional<std::string>&& target_ext,
                 std::string&&                uuid );

    ProjectNorm( ProjectNorm&& ) = default;

};

auto norm_paths( Project const& p ) -> ProjectNorm;

ProjectNorm read_norm( fs::path const&  file,
                       std::string_view platform );

std::ostream& operator<<( std::ostream&      out,
                          ProjectNorm const& p );

} // namespace project
