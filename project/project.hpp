/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#pragma once

#include "fs.hpp"
#include "attr.hpp"
#include "non-copyable.hpp"

#include <iostream>
#include <string_view>

namespace project {

struct Project : public util::non_copyable {

    ProjectAttr const& attr() const { return m_attr; }

    static Project read( fs::path const&  file,
                         std::string_view platform,
                         fs::path const&  base = "" );

private:
    Project( ProjectAttr&& );

    ProjectAttr m_attr;
};

std::ostream& operator<<( std::ostream&  out,
                          Project const& p );

} // namespace project
