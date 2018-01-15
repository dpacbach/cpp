/****************************************************************
* IO related utilities
****************************************************************/
#pragma once

#include "fs.hpp"
#include "types.hpp"

namespace util {

// Read  a  file in its entirety into a vector of chars. This may
// be a bit less efficient than possible because the vector, when
// created, will initialize all of its bytes  to  zero  which  we
// don't actually need.
std::vector<char> read_file( fs::path p );

// Open the file, truncate it,  and  write  given  vector  to  it.
void write_file( fs::path const& p, std::vector<char> const& v );

// Read a text file into a string in its entirety.
std::string read_file_str( fs::path p );

// Read  a text file into a string in its entirety and then split
// it into lines.
StrVec read_file_lines( fs::path p );

} // namespace std
