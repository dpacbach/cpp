/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#pragma once

#include "fs.hpp"
#include "project_raw.hpp"

namespace project {

struct Project : ProjectRaw {

    Project( std::vector<fs::path>&&      cl_includes,
             std::vector<fs::path>&&      cl_compiles,
             std::vector<fs::path>&&      search_paths,
             fs::path&&                   int_dir,
             fs::path&&                   out_dir,
             std::string&&                project_name,
             std::optional<std::string>&& target_name,
             std::optional<std::string>&& target_ext,
             std::string&&                uuid );

    Project( Project&& ) = default;

    static Project read( fs::path const&  file,
                         std::string_view platform );

    static Project read( fs::path const&  file,
                         fs::path const&  base,
                         std::string_view platform );

};

std::ostream& operator<<( std::ostream&  out,
                          Project const& p );

} // namespace project
