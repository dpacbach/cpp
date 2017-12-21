/****************************************************************
* Mock Preprocessor
****************************************************************/
#pragma once

#include "fs.hpp"
#include "types.hpp"

#include <optional>
#include <string_view>

namespace project {

OptPath parse_include( std::string const& sv );

PathVec parse_includes( fs::path const& file );

PathVec find_sources( fs::path where, fs::path base );

} // namespace project
