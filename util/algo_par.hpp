/****************************************************************
* Parallel Algorithms
****************************************************************/
#pragma once

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
    using ResT = std::decay_t<decltype(
        func( *std::begin( range ) )
    )>;
    std::vector<ResT> res; res.reserve( range.size() );
    for( auto const& x : range )
        res.emplace_back( func( x ) );
    return res;
}

} // namespace util
