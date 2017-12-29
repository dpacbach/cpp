/****************************************************************
* Mock Preprocessor
****************************************************************/
#pragma once

#include "fs.hpp"
#include "graph.hpp"
#include "project.hpp"
#include "solution.hpp"
#include "types.hpp"

namespace project {

// Parse  a  single  line for an include statement, returning the
// file name/path if parsing  succeeded.  Note,  file/path is not
// resolved or confirmed to exist. This is just a parser.
OptPath parse_include( std::string const& sv );

// Open a file and apply the above function to each line.
PathVec parse_includes( fs::path const& file );

// Data structure we will used to hold a list of all  files  that
// are  considered  relevant  as well as their include statements
// (which may  be  resolved  or  not  depending  on  the context).
using GlobalIncludeMap    = std::unordered_map<fs::path, PathVec>;
using GlobalRefIncludeMap = std::unordered_map<PathCRef,
                                               PathCRefVec>;
// Find all source files under the `from` paths, open them up and
// parse all the include statements  in  them,  then return a map
// where key is the file path relative to base_path and the value
// is a list of parsed (but not resolved) include files.
GlobalIncludeMap build_sources( PathVec const& from,
                                fs::path       base_path );

// Same  as  above  but will attempt to divide the work among mul-
// tiple threads. Zero value for jobs means that it will take max
// threads available on system.
GlobalIncludeMap build_sources_par( PathVec const& from,
                                    fs::path       base_path,
                                    int            jobs = 0 );

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

// Same as above but looks up the `current path` and list  of  in-
// cluded relative paths from the global map.
PathCRefVec resolves( GlobalIncludeMap const& global,
                      PathVec          const& search_paths,
                      fs::path         const& file );

// This will essentially run the preprocessor on  a  project  and
// produce the final directed graph. Note that the directed graph
// will  contain  only  files encountered starting from the speci-
// fied sources, and all references  to  files will be references
// to  within  the  global  map. Note that sources are also refer-
// ences to within the global map.
util::DirectedGraph<PathCRef>
preprocess( GlobalIncludeMap const& global,
            PathVec          const& search_paths,
            PathCRefVec      const& sources );

// Same as above, but takes a project, and writes out the results
// to the Cl.read.1.tlog file in the  intermediate  folder.  Will
// return the file name that it wrote to.
fs::path preprocess_project( GlobalIncludeMap const& global,
                             fs::path         const& base,
                             Project          const& project );

// Will  open  the  solution,  parse  it, parse all project files
// therein, and then call preprocess on each project.
void preprocess_solution( GlobalIncludeMap const& global,
                          fs::path         const& base,
                          Solution         const& solution,
                          int                     jobs = 0 );

// Will  open  the  solution,  parse  it, parse all project files
// therein, and then call preprocess on each project.
void preprocess_solution( GlobalIncludeMap const& global,
                          fs::path         const& base,
                          fs::path         const& solution,
                          std::string_view        platform,
                          int                     jobs = 0 );

} // namespace project
