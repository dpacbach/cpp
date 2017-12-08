/****************************************************************
* filesystem related utilities
****************************************************************/
#pragma once

#include "types.hpp"

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

/* Implemenation of the  lexically_relative  function.  Find  the
 * relative  path between the given path and base path without re-
 * gard  to  whether or not it exists (and do so without touching
 * the  filesystem  at  all; hence we also don't follow symlinks).
 * The C++17 filesystem library has this  method as a member func-
 * tion of the path class, but it has not yet been implemented in
 * gcc at the time of this writing.  Once gcc 8 is released, then
 * this function can probably be deleted and the usage of it  can
 * be replaced with p.lexicaly_relative(). */
fs::path lexically_relative( fs::path const& p,
                             fs::path const& base );

// This  is  a  simplified version of lexically_relative which as-
// sumes (assumptions are not verified for efficiency!) that both
// input  paths  are  either  absolute or both are relative, that
// both are in normal form, and that the base path has no  double
// dots. If you call this function  with  those  assumptions  vio-
// lated then it's not certain what you will get.
fs::path lexically_relative_fast( fs::path const& p,
                                  fs::path const& base );

// Flip any backslashes to forward slashes.
std::string fwd_slashes( std::string_view in );

// Flip any backslashes to forward slashes.
StrVec fwd_slashes( StrVec const& v );

// Constructs  a path from a pair of iterators to path components.
// Didn't see this available in  the  standard,  but  could  have
// missed it. The name is meant  for  it  to look like a construc-
// tor, even though it isn't.
fs::path path_( fs::path::const_iterator b,
                fs::path::const_iterator e );

} // namespace util
