/****************************************************************
* Type aliases
****************************************************************/
#pragma once

// Aliases here are made in the global namespace and  are  simply
// to save typing.

#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

using OptStr = std::optional<std::string>;
using StrVec = std::vector<std::string>;
using SVVec  = std::vector<std::string_view>;
