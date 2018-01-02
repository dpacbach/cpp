/****************************************************************
* IO related utilities
****************************************************************/
#pragma once

#include "fs.hpp"
#include "types.hpp"

namespace util {

// Read a file in its entirety into  a vector of chars. For imple-
// mentation idea, see on stackoverflow:
//   "c-vector-that-doesnt-initialize-its-members"
std::vector<char> read_file( fs::path p );

// Read a text file into a string in its entirety.
std::string read_file_str( fs::path p );

// Read  a text file into a string in its entirety and then split
// it into lines.
StrVec read_file_lines( fs::path p );

} // namespace std
