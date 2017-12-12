/****************************************************************
* Project File Parser
****************************************************************/
#pragma once

#include "attr.hpp"

#include <experimental/filesystem>
#include <string_view>

namespace fs = std::experimental::filesystem;

namespace project {

// This will parse the XML file and  then traverse it to find the
// data  necessary to populate the ProjectAttr structure. However,
// it will perform only the  minimum  amount of processing on the
// data necessary to populate the ProjectAttr struct.
ProjectAttr parse( fs::path const&  file,
                   std::string_view platform );

} // namespace project
