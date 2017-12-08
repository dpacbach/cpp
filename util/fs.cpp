/****************************************************************
* filesystem related utilities
****************************************************************/
#include "macros.hpp"
#include "fs.hpp"

#include <algorithm>

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
 * Note  that  the code doesn't perform these actions in the same
 * order as the steps below, but the overall effect should be the
 * same.
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
    bool is_abs = p.is_absolute(), is_rel = p.is_relative();
    fs::path res;
    for( auto c : p ) {
        if( c == "." )
            // The single dot we can always skip  at  this  stage.
            // The only time we need  one  is  when we are normal-
            // izing an empty path, which must  be  converted  to
            // single dot, however we will do  that  at  the  end.
            continue;
        if( c == ".." ) {
            // If we have encountered  a  ..  and  if  we have at
            // least one component in the  result  so far then we
            // can  eliminate  it. We need to guard this with one
            // additional  check  (has_parent_path)  in  the case
            // that we have an absolute  path because an absolute
            // path can consist just  of  the  root  (/) but will
            // still report having a file name (/) which we don't
            // want to remove.
            if( res.has_filename() &&
               (res.has_parent_path() || is_rel) ) {
                // If we are  a  relative  path  then  there is a
                // chance that the  filename  could  be .. (since
                // those are allowed to accumulate at  the  start
                // of  a  relative path), but which we don't want
                // to remove.
                if( res.filename() != ".." ) {
                    res = res.parent_path();
                    continue;
                }
            }
            if( is_abs )
                // On an abs path we can ignore .. at  the  begin-
                // ning of paths. We  know  that  we're at the be-
                // ginning because, if we we  not  at  the  begin-
                // ning,  the above block would have continued be-
                // cause a) we'd have a  parent path, and b) that
                // parent path would not be a .. because previous
                // ..'s  get  filtered  out  on abs paths (due to
                // this very piece of logic).
                continue;
            // It's a relative path, so that  means  we  need  to
            // keep ..'s at the beginning of paths,  so  we  fall
            // through.
        }
        res /= c;
    }
    // Result will never be empty.
    return res.empty() ? "." : res;
}

// Flip any backslashes to foward slashes.
string fwd_slashes( string_view in ) {
    string out( in );
    replace( begin( out ), end( out ), '\\', '/' );
    return out;
}

// Flip any backslashes to forward slashes.
StrVec fwd_slashes( StrVec const& v ) {
    vector<string> res( v.size() );
    auto resolve = []( string_view sv ) {
        return fwd_slashes( sv );
    };
    transform( begin( v ), end( v ), begin( res ), resolve );
    return res;
}

} // util
