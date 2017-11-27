/****************************************************************
* Project & Solution File Parsers
****************************************************************/
#include "pugixml.hpp"

#include <experimental/filesystem>
#include <iostream>
#include <string_view>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace project {

struct Project {

    Project( std::vector<std::string>&& cl_includes,
             std::vector<std::string>&& cl_compiles,
             std::vector<std::string>&& search_paths );

    Project( Project&& ) = default;

    std::vector<std::string> const cl_includes;
    std::vector<std::string> const cl_compiles;
    std::vector<std::string> const search_paths;

};

Project read( fs::path file, std::string_view platform );

std::ostream& operator<<( std::ostream& out, Project const& p );

} // namespace project
