/****************************************************************
* Parallel Algorithms
****************************************************************/
#pragma once

#include "error.hpp"
#include "util.hpp"

#include <type_traits>
#include <vector>

namespace util {

/* Parallel  map:  apply a function to elements in a range in par-
 * allel.  This  is  being  implemented until the parallel STL be-
 * comes available. Note: the range here expects to have a size()
 * function. */
template<typename FuncT, typename RangeT>
auto map_par( FuncT func, RangeT const& range )
{
    // Get  the  underlying value type held by the range and then
    // get the type of result after calling the  function  on  it,
    // stripping away references and const.
    using PayloadT = std::decay_t<decltype(
        func( *std::begin( range ) )
    )>;

    // The results of calling the function  will  be  held  in  a
    // vector of variants, the  variants  being  to  contain  any
    // error  information  if  the  function  call  fails  (a.k.a.
    // throws).
    using Res = Result<PayloadT>;
    std::vector<Res> res; res.reserve( range.size() );

    auto func_wrapper = [&func]( auto const& arg ) -> Res {
        try {
            return Res{ func( arg ) };
        } catch( std::exception const& e ) {
            return Res{ Error{ e.what() } };
        } catch( ... ) {
            return Res{ Error{ "unknown exception" } };
        }
    };

    for( auto const& x : range )
        res.emplace_back( func_wrapper( x ) );

    return res;
}

} // namespace util
