/****************************************************************
* Project & Solution File Parsers
****************************************************************/
#pragma once

#include "project_attr.hpp"
#include "types.hpp"

#include <experimental/filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

/****************************************************************
* ProjectRaw
****************************************************************/
// Will  hold  the  raw, unprocessed contents of the project file,
// where "contents" mean anything in the ProjectAttr
// struct. Unprocessed means basically that there will be no path
// manipulation.
struct ProjectRaw {

    ProjectRaw( ProjectAttr&& );

    ProjectRaw( ProjectRaw&& )                  = default;
    ProjectRaw& operator=( ProjectRaw&& )       = default;

    ProjectRaw( ProjectRaw const& )             = delete;
    ProjectRaw& operator=( ProjectRaw const&  ) = delete;

    static ProjectRaw read( fs::path const&  file,
                            std::string_view platform );

    ProjectAttr attr;
};

std::ostream& operator<<( std::ostream& out,
                          ProjectRaw    const& p );

} // namespace project
