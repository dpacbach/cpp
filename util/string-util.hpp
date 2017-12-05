/****************************************************************
* String utilities
****************************************************************/
#pragma once

#include <experimental/filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace util {

// Strip all blank space off of a string view and return
// a new one.
std::string_view strip( std::string_view sv );

// Split a string on a character.
std::vector<std::string_view>
split( std::string_view sv, char c );

// Split a string, strip all elements, and remove empty strings
// from result.
std::vector<std::string_view>
split_strip( std::string_view sv, char c );

// Convert element type.
std::vector<std::string>
to_strings( std::vector<std::string_view> const& svs );

// Convert string to path
fs::path to_path( std::string_view sv );

// Convert element type.
std::vector<fs::path>
to_paths( std::vector<std::string> const& ss );

// Flip any backslashes to forward slashes.
std::string fwd_slashes( std::string_view in );

// Flip any backslashes to forward slashes.
std::vector<std::string>
fwd_slashes( std::vector<std::string>& v );

}
