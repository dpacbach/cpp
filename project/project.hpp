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

    // We need this to  support  initializing  vectors with empty
    // projects for use in parallelization algorithms.
    Project() {}

    ProjectAttr const& attr() const { return m_attr; }
    fs::path    const& path() const { return m_path; }

    static Project read( fs::path const&  file,
                         std::string_view platform,
                         fs::path const&  base = "" );

private:
    Project( fs::path const& p, ProjectAttr&& );

    fs::path    m_path;
    ProjectAttr m_attr;
};

std::ostream& operator<<( std::ostream&  out,
                          Project const& p );

/****************************************************************
* Derived Quantities
****************************************************************/
auto tlog_path( Project const& p ) -> fs::path;
// These four are the exe/dll/lib/exp/pdb  files that will appear
// in  the  main  output folder. They will all have the same name,
// just different extension. They have optional as return because
// not all projects have a target binary.
auto trg_path ( Project const& p ) -> OptPath;
auto lib_path ( Project const& p ) -> OptPath;
auto pdb_path ( Project const& p ) -> OptPath;
auto exp_path ( Project const& p ) -> OptPath;
// lasbuildstate & unsuccessfulbuild are in the tlog folder.
auto lbs_path ( Project const& p ) -> fs::path;
auto ubs_path ( Project const& p ) -> fs::path;

} // namespace project
