/****************************************************************
* Adding and Removing from Project Files
****************************************************************/
#pragma once

#include "types.hpp"

namespace project {

// Open the project file and insert the given  source  file  into
// the list of ClCompile's of the project. Note, slashes will  al-
// ways  be converted to back slashes when writing paths into the
// project file, regardless of platform.  The  source file, if it
// is  a  relative  path, is interpreted as being relative to CwD
// (not relative to the  project  file  location).  If the source
// file  already  exists in the project file (by case-insensitive
// comparison) then an exception will be thrown. The source  file
// does not need to exist.
void add_2_vcxproj( fs::path const& project_file,
                    fs::path const& src_file );

// Open  the  filters  file and insert the given source file into
// it. First we search the filters  file for any other files that
// are  in the same folder as the given one and, if there are, we
// will obtain the name of the filter in that way by choosing the
// first filter that we encounter that contains at least one file
// in the same folder  as  src_file.  Otherwise,  we will use the
// first filter that we find. The source file, if it  is  a  rela-
// tive path, is interpreted as being relative to CWD  (not  rela-
// tive to the project file location). If the source file already
// exists in the  filters  file  (by case-insensitive comparison)
// then an exception will be thrown. The  source  file  does  not
// need to exist.
void add_2_filters( fs::path const& filters_file,
                    fs::path const& src_file );

// This  is simply for convenience; it will call add_2_project on
// the given project file,  then  will  construct  a filters file
// path by appending ".filters" to the  end  of the name and will
// then  call  add_2_filters  on  the filters file. If either the
// project file or filters file does  not  exist,  and  exception
// will be thrown. The source file does not need to exist.
void add_2_project( fs::path const& project_file,
                    fs::path const& src_file );

// Open the given file, which could be either a project file or a
// filters file, and remove any ClCompile elements from within it
// that reference the given source file.  Note  that  the  source
// file,  if it is a relative path, is interpreted as a path rela-
// tive  to  CWD.  The source file path and the various ClCompile
// paths  encountered  in  the file will be normalized before per-
// forming  a case-insensitive comparison to find a match. If the
// source file is not found in the file then an exception will be
// thrown.  Also,  the source file need not exist in the file sys-
// tem; this is to accomodate  cases  where  we  want to remove a
// file from a project file  because  the  user  has  it  locally
// deleted in their working copy.
void rm_from( fs::path const& file,
              fs::path const& src_file );

} // namespace project
