/****************************************************************
* Project & Solution File Parsers
****************************************************************/
#pragma once

#include "pugixml.hpp"

#include <experimental/filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

struct Project {

    Project( std::vector<fs::path>&&      cl_includes,
             std::vector<fs::path>&&      cl_compiles,
             std::vector<fs::path>&&      search_paths,
             fs::path&&                   int_dir,
             std::string&&                project_name,
             std::optional<std::string>&& target_name,
             std::optional<std::string>&& target_ext );

    Project( Project&& )      = default;
    Project( Project const& ) = delete;
    Project()                 = delete;

    std::string to_string() const;

    std::vector<fs::path>      const cl_includes;
    std::vector<fs::path>      const cl_compiles;
    std::vector<fs::path>      const search_paths;
    fs::path                   const int_dir;
    std::string                const project_name;
    std::optional<std::string> const target_name;
    std::optional<std::string> const target_ext;

};

auto read( fs::path file, std::string_view platform ) -> Project;

std::ostream& operator<<( std::ostream& out, Project const& p );

} // namespace project
