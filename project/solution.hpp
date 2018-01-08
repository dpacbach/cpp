/****************************************************************
* Solution File Parsing and Processing
****************************************************************/
#pragma once

#include "project.hpp"
#include "types.hpp"
#include "non-copyable.hpp"

#include <experimental/filesystem>
#include <map>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

/****************************************************************
* SolutionFile
****************************************************************/
struct SolutionFile : public util::non_copyable {

    SolutionFile( PathVec&& projects );

    // Reads  in  the  solution  file and parses it for any lines
    // that declare projects, and then  extracts the project file
    // location (vcxproj) for each project.
    static SolutionFile read( std::istream& in );
    // This one reads from a file.
    static SolutionFile read( fs::path const& file );

    PathVec const& projects() const { return m_projects; }

private:
    // Holds only paths to projects, and those paths will be  rel-
    // ative to the folder containing the solutiont file.
    PathVec m_projects;
};

// Mainly for debugging.
std::ostream& operator<<( std::ostream&       out,
                          SolutionFile const& s );

/****************************************************************
* Solution
****************************************************************/
struct Solution : public util::non_copyable {

    using map_type = std::map<fs::path, Project>;

    Solution( map_type&& projects );

    // Read in a solution file, parse  it  to  get  the  list  of
    // projects that it contains, then  read in each project file
    // and parse it completely.
    static Solution read( std::istream&    in,
                          fs::path const&  sln_parent,
                          std::string_view platform,
                          fs::path const&  base = "" );
    // This one reads from a file.
    static Solution read( fs::path const&  file,
                          std::string_view platform,
                          fs::path const&  base = "" );

    map_type const& projects() const { return m_projects; }

private:
    // Holds path to vcxproj file and full project data.
    map_type m_projects;
};

// Mainly for debugging.
std::ostream& operator<<( std::ostream&   out,
                          Solution const& s );

} // namespace project
