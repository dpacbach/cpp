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
    fs::path    const& path() const { return m_path; }

    static Project read( fs::path const&  file,
                         std::string_view platform,
                         fs::path const&  base = "" );

private:
    Project( ProjectAttr&& );

    fs::path    m_path;
    ProjectAttr m_attr;
};

std::ostream& operator<<( std::ostream&  out,
                          Project const& p );

/****************************************************************
* Derived Quantities
****************************************************************/
auto tlog_path   ( Project const& attr ) -> fs::path;
auto target_path ( Project const& attr ) -> OptPath;
auto lib_path    ( Project const& attr ) -> OptPath;
auto pdb_path    ( Project const& attr ) -> OptPath;
auto exp_path    ( Project const& attr ) -> OptPath;

auto lbs_path    ( Project const& attr ) -> fs::path;
auto ubs_path    ( Project const& attr ) -> fs::path;

auto srcs_paths  ( Project const& attr ) -> PathVec;

} // namespace project
