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

/* Implementation of lexically_normal.  Put  a  path  into normal
 * form without regard to whether  or  not  it  exists (and do so
 * without touching the filesystem  at  all;  hence we also don't
 * follow symlinks). The C++17 filesystem library has this method
 * as a member function of  the  path  class,  but it has not yet
 * been implemented in gcc at the time of this writing. Once  gcc
 * 8 is released, then this function can probably be deleted  and
 * the  usage of it can be replaced with p.lexicaly_normal(). How-
 * ever, note that  the  implementation  of  this  function  will
 * likely be slightly different from  the  one in the standard li-
 * brary.  In  particular,  at  the  moment,  it does not convert
 * slashes  using  make_preferred. Note that the code doesn't per-
 * form these actions in the same  order  as the steps below, but
 * the overall effect should be the same.
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

/* Implemenation of the  lexically_relative  function.  Find  the
 * relative  path between the given path and base path without re-
 * gard  to  whether or not it exists (and do so without touching
 * the  filesystem  at  all; hence we also don't follow symlinks).
 * The C++17 filesystem library has this  method as a member func-
 * tion of the path class, but it has not yet been implemented in
 * gcc at the time of this writing.  Once gcc 8 is released, then
 * this function can probably be deleted and the usage of it  can
 * be replaced with p.lexicaly_relative(). However, note that the
 * implementation of this function will likely  be  slightly  dif-
 * ferent from the one in the standard library.
 *
 * Return value: empty path on error, result otherwise.
 *
 * NOTE: this function implementation does  not  normalize  paths
 * and will generally not return them in normal form, even if the
 * inputs are both in normal form.
 *
 * NOTE:  this function doesn't always work right in certain edge
 * cases involving the presence of dot-dot's in the base path and
 * dot-dot's immediately proceeding  a  root  folder...  it is ad-
 * vised to avoid supplying these  kinds  of arguments to be safe.
 * In  some  cases it tries to detect these scenarios and will re-
 * turn an error (empty  path),  but  it's  probably not reliable.
 *
 * From en.cppreference.com/w/cpp/filesystem/path/lexically_normal
 * the algorithm is as follows:
 *
 *   if (root_name()   != base.root_name() ) ||
 *      (is_absolute() != base.is_absolute() ||
 *      (!has_root_directory() && base.has_root_directory())
 *        returns a default-constructed path.
 *
 *   Otherwise,  determines the first mismatched element of *this
 *   and base as if by:
 *
 *      auto [a, b] = mismatch(begin(), end(),
 *                             base.begin(), base.end())
 *
 *      if  a  ==  end()  and b == base.end(), returns path(".");
 *
 *   Otherwise,  if the number of dot-dot filename elements in [b,
 *   base.end())  is greater than the number of filename elements
 *   that are neither dot nor  dot-dot,  returns  a  default  con-
 *   structed path. otherwise returns an object composed from:  a
 *   default-constructed path() followed by as many  applications
 *   of operator/=(path("..")) as there were filename elements in
 *   [b,  base.end())  that  are  neither  dot  nor dot-dot minus
 *   number  of  dot-dot  elements in that range, followed by one
 *   application of operator/= for each element in the  half-open
 *   range [a, end())
 */
fs::path lexically_relative( fs::path const& p,
                             fs::path const& base ) {
    if( (p.root_name()   != base.root_name())   ||
        (p.is_absolute() != base.is_absolute()) ||
        (!p.has_root_directory() && base.has_root_directory()) )
         return {};

    auto [a, b] = mismatch( begin( p ), end( p ),
                            begin( base ), end( base ) );

    if( a == end( p ) && b == end( base ) )
        return { "." };

    auto n_dd = count( b, end( base ), ".." );
    auto n_d  = count( b, end( base ), "."  );
    auto dist = distance( b, end( base ) );
    auto n_f  = dist - n_dd - n_d;
    auto n_r  = n_f - n_dd;

    ASSERT_( dist >= (n_d + n_dd) );

    // The next test is to be sure that we return an empty path
    // (meaning error I suppose) in situations like the following,
    // where, given that we can't query the filesystem, we cannot
    // be sure of the correct answer:
    //   lexically_relative( ".", "a/.." ) == "."; // ok
    //   lexically_relative( ".", "../a" ) == ???; // bad
    // Though this only arises with relative paths.
    if( n_dd > 0 ) {
        auto norm = lexically_normal( path_( b, end( base ) ) );
        if( count( begin( norm ), end( norm ), ".." ) > 0 )
            return {};
    }

    fs::path res;
    while( n_r-- > 0 )
        res /= fs::path( ".." );

    for( auto i = a; i != end( p ); ++i )
        res /= *i;

    // Result, which at this  point  is  interpreted as "correct"
    // will be changed to normal form if it is empty, that way we
    // can use "empty" to mean "couldn't find solution".
    return res.empty() ? "." : res;
}

// This  is  a  simplified version of lexically_relative which as-
// sumes (assumptions are not verified for efficiency!) that both
// input  paths  are  either  absolute or both are relative, that
// both are in normal form, and that the base path has no  double
// dots. If you call this function  with  those  assumptions  vio-
// lated then it's not certain what you will get.
fs::path lexically_relative_fast( fs::path const& p,
                                  fs::path const& base ) {

    auto [a, b] = mismatch( begin( p ), end( p ),
                            begin( base ), end( base ) );

    fs::path res;
    for( auto i = b; i != end( base ); ++i )
        res /= fs::path( ".." );

    for( auto i = a; i != end( p ); ++i )
        res /= *i;

    // Result, which at this  point  is  interpreted as "correct"
    // will be changed to normal form if it is empty, that way we
    // can use "empty" to mean "couldn't find solution".
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

// Constructs  a path from a pair of iterators to path components.
// Didn't see this available in  the  standard,  but  could  have
// missed it.
fs::path path_( fs::path::const_iterator b,
				fs::path::const_iterator e ) {
    fs::path res;
    for( auto i = b; i != e; ++i )
        res /= *i;
    return res;
}

} // util
