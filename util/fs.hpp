/****************************************************************
* filesystem related utilities
****************************************************************/
#pragma once

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace util {

// Attempts to mimic the general behavior of the linux `realpath`
// function, but in a cross platform  way. Note that this will re-
// solve symlinks as well as make the path absolute.
fs::path realpath( fs::path const& p );

// This will put the path (which  may not exist) into normal form
// and preserving absolute/relative nature.
fs::path normpath( fs::path const& p );

// This  is  like normpath except that it makes the path absolute
// (relative to cwd) if it is not already).
fs::path absnormpath( fs::path const& p );

/* Put  a  path into normal form without regard to whether or not
 * it  exists  (and  do so without touching the filesystem at all.
 * The C++17 filesystem library has this  method as a member func-
 * tion of the path class, but it has not yet been implemented in
 * gcc at the time of this writing.  Once gcc 8 is released, then
 * this  function  can  be  deleted and the usage of it can be re-
 * placed with p.lexicaly_normal(). */
fs::path lexically_normal( fs::path const& p );

} // namespace util
