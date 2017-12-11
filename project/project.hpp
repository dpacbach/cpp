/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#pragma once

#include "fs.hpp"
#include "project_attr.hpp"

#include <iostream>
#include <string_view>

namespace project {

struct Project {

    Project( ProjectAttr&& );

    Project( Project&& )                  =  default;
    Project& operator=( Project&& )       =  default;

    Project( Project const& )             =  delete;
    Project& operator=( Project const&  ) =  delete;

    static Project read( fs::path const&  file,
                         std::string_view platform );

    static Project read( fs::path const&  file,
                         fs::path const&  base,
                         std::string_view platform );

    ProjectAttr attr;
};

std::ostream& operator<<( std::ostream&  out,
                          Project const& p );

} // namespace project
