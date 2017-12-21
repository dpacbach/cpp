/****************************************************************
* Mock Preprocessor
****************************************************************/
#pragma once

#include "fs.hpp"
#include "types.hpp"

namespace project {

// Parse  a  single  line for an include statement, returning the
// file name/path if parsing  succeeded.  Note,  file/path is not
// resolved or confirmed to exist. This is just a parser.
OptPath parse_include( std::string const& sv );

// Open a file and apply the above function to each line.
PathVec parse_includes( fs::path const& file );

// Data structure we will used to hold a list of all  files  that
// are considered relevant as well as  their  include  statements.
using GlobalIncludeMap = std::unordered_map<
    fs::path,
    std::vector<fs::path>
>;

// Find  all source files under the `from` path, open them up and
// parse all the include statements  in  them,  then return a map
// where key is the file path relative to base_path and the value
// is a list of parsed (but not resolved) include files.
GlobalIncludeMap build_sources( fs::path from,
                                fs::path base_path );

} // namespace project
