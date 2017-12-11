/****************************************************************
* Project & Solution File Parsers
****************************************************************/
#pragma once

#include "project_attr.hpp"
#include "types.hpp"
#include "non-copyable.hpp"

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
struct ProjectRaw : public util::non_copyable {

    ProjectRaw( ProjectAttr&& );

    ProjectAttr const& attr() const { return m_attr; }

    static ProjectRaw read( fs::path const&  file,
                            std::string_view platform );

private:
    ProjectAttr m_attr;
};

std::ostream& operator<<( std::ostream& out,
                          ProjectRaw    const& p );

} // namespace project
