/****************************************************************
* filesystem related utilities
****************************************************************/
#include "macros.hpp"
#include "fs.hpp"

#include <iostream>

using namespace std;

namespace util {

// Attempts to mimic the general behavior of the linux `realpath`
// function, but in a cross platform  way. Note that this will re-
// solve symlinks as well as make the path absolute.
fs::path realpath( fs::path const& p ) {
    return fs::canonical( fs::absolute( p ) );
}

// This will put the path (which  may not exist) into normal form
// and preserving absolute/relative nature.
fs::path normpath( fs::path const& p ) {
    return util::lexically_normal( p );
}

// This  is  like normpath except that it makes the path absolute
// (relative to cwd) if it is not already).
fs::path absnormpath( fs::path const& p ) {
    return util::lexically_normal( fs::absolute( p ) );
}

/* Put  a  path into normal form without regard to whether or not
 * it  exists  (and  do so without touching the filesystem at all.
 * The C++17 filesystem library has this  method as a member func-
 * tion of the path class, but it has not yet been implemented in
 * gcc at the time of this writing.  Once gcc 8 is released, then
 * this function can probably be deleted and the usage of it  can
 * be replaced with p.lexicaly_normal(). However, note  that  the
 * implementation of this function will likely  be  slightly  dif-
 * ferent from the one in the standard library. In particular, at
 * the moment, it does  not  convert slashes using make_preferred.
 *
 * From http://en.cppreference.com/w/cpp/filesystem/path, a  path
 * can be normalized by following this algorithm:
 *
 *  1) If the path is empty, stop (normal form of an  empty  path
 *     is an empty path)
 *  2) Replace each directory-separator  (which  may  consist  of
 *     multiple slashes) with  a single path::preferred_separator.
 *  3) Replace each slash character in the root-name with
 *     path::preferred_separator.
 *  4) Remove each dot and any immediately following
 *     directory-separator.
 *  5) Remove each non-dot-dot filename immediately followed by a
 *     directory-separator and a dot-dot, along with  any  immedi-
 *     ately following directory-separator.
 *  6) If there is  root-directory,  remove  all dot-dots and any
 *     directory-separators immediately following them.
 *  7) If the  last  filename  is  dot-dot,  remove  any trailing
 *     directory-separator.
 *  8) If the path is empty, add  a  dot (normal form of ./ is .)
 */
fs::path lexically_normal( fs::path const& p ) {
    ASSERT( !p.has_root_name(), "path " << p << " has a root"
                                "name which is not supported." );
    fs::path res;
    for( auto c : p ) {
        if( c == "." )
            continue;
        if( c == ".." ) {
            if( res.has_parent_path() )
                res = res.parent_path();
            continue;
        }
        res /= c;
    }
    return res.empty() ? "." : res;
}

} // util
