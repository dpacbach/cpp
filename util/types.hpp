/****************************************************************
* Type aliases
****************************************************************/
#pragma once

// TODO: need to figure out how to forward declare the relevant
//       classes.

// Aliases here are made in the global namespace and  are  simply
// to save typing.

#include <experimental/filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace fs = std::experimental::filesystem;

using OptStr  = std::optional<std::string>;
using OptPath = std::optional<fs::path>;
using StrVec  = std::vector<std::string>;
using SVVec   = std::vector<std::string_view>;
using PathVec = std::vector<fs::path>;
