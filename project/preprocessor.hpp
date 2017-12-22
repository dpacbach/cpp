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
using GlobalIncludeMap = std::unordered_map<fs::path, PathVec>;

// Find  all source files under the `from` path, open them up and
// parse all the include statements  in  them,  then return a map
// where key is the file path relative to base_path and the value
// is a list of parsed (but not resolved) include files.
GlobalIncludeMap build_sources( fs::path from,
                                fs::path base_path );

// Take  a  relative  path and a list of search paths and resolve
// the path using (approximately) the algorithm that the compiler
// would  use, which means to iterate through the search paths in
// order, construct a candidate path  from  each,  and then to to
// stop as soon as a match is  found, where a match is defined as
// a file that is found  in  the  global  file map. Note that, as
// some compilers do, we first  search  in the current_path which
// is supposed to represent the  folder  containing the file that
// included this file under examination.  Note  that the paths in-
// volved are expected to all be in normal form.
//
// When a match is  found  it  will  be  returned  as a reference
// (wrapper) to the path stored inside the global mapping,  other-
// wise nullopt.
OptCRef<fs::path> resolve( GlobalIncludeMap const& global,
                           fs::path         const& current_path,
                           PathVec          const& search_paths,
                           fs::path         const& relative );

// Runs  the  `resolve`  function  over  a list of relative paths.
// paths that do not resolve are simply ignored.
PathCRefVec resolves( GlobalIncludeMap const& global,
                      fs::path         const& current_path,
                      PathVec          const& search_paths,
                      PathVec          const& relatives );

} // namespace project
