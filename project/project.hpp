/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#pragma once

#include "fs.hpp"
#include "project_raw.hpp"

namespace project {

struct Project : ProjectRaw {

    Project( ProjectAttributes&& );

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
