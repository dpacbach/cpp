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

} // namespace project
