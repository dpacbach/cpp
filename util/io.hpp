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
std::vector<char> read_file( fs::path const& p );

// Open the file, truncate it,  and  write  given  vector  to  it.
void write_file( fs::path const& p, std::vector<char> const& v );

// We should not need this function  because  the  filesystem  li-
// brary provides fs::copy_file which would ideally be better  to
// use. However, it was observed at  the  time  of  this  writine
// (with  gcc  7.2)  that  the fs::copy_file method was not faith-
// fully copying files  on  Windows  that  were  created on Linux.
// Specifically, it was observed that,  copying  a text file with
// Linux  line endings on Windows resulted in a new file with Win-
// dows line endings, which is  not  desired.  Hence we have this
// function which will copy the file in  binary  mode  faithfully.
void copy_file( fs::path const& from, fs::path const& to );

// Read a text file into a string in its entirety.
std::string read_file_str( fs::path p );

// Read  a text file into a string in its entirety and then split
// it into lines.
StrVec read_file_lines( fs::path p );

} // namespace std
