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

struct ProjectRaw {

    ProjectRaw( std::vector<fs::path>&&      cl_includes,
                std::vector<fs::path>&&      cl_compiles,
                std::vector<fs::path>&&      search_paths,
                fs::path&&                   int_dir,
                fs::path&&                   out_dir,
                std::string&&                project_name,
                std::optional<std::string>&& target_name,
                std::optional<std::string>&& target_ext,
                std::string&&                uuid );

    ProjectRaw( ProjectRaw const& ) = delete;
    ProjectRaw( ProjectRaw&& )      = default;

    ProjectRaw& operator=( ProjectRaw const&  ) = delete;
    ProjectRaw& operator=( ProjectRaw const&& ) = delete;

    std::string to_string() const;

    std::string tlog_name() const;

    static ProjectRaw read( fs::path const&  file,
                            std::string_view platform );

    std::vector<fs::path>      const cl_includes;
    std::vector<fs::path>      const cl_compiles;
    std::vector<fs::path>      const search_paths;
    fs::path                   const int_dir;
    fs::path                   const out_dir;
    std::string                const project_name;
    std::optional<std::string> const target_name;
    std::optional<std::string> const target_ext;
    std::string                const uuid;

};

std::ostream& operator<<( std::ostream&     out,
                          ProjectRaw const& p );

} // namespace project
