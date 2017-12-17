/****************************************************************
* Bi-directional Map Classes
****************************************************************/
#pragma once

#include "non-copyable.hpp"

#include <algorithm>
#include <optional>
#include <vector>

namespace util {

template<typename T>
class BDIndexMap : util::non_copyable {

    BDIndexMap( std::vector<T>&& data, bool is_sorted = false );

    size_t size() const { return data.size(); }

    std::optional<T const&> val( size_t n ) const;

    std::optional<size_t> key( T const& val ) const;

private:

    std::vector<T> data;
};

template<typename T>
BDIndexMap::BDIndexMap( std::vector<T>&& data, bool is_sorted )
    : data( move( data ) ) {

    if( !is_sorted )
        std::sort( begin( data ), end( data ) );
}

template<typename T>
std::optional<size_t> BDIndexMap::key( T const& val ) {

    auto i = std::lower_bound( begin( data ), end( data ), val );

    if( i != end( data ) && *i == val )
        return i - begin( data );

    return nullopt;
}

template<typename T>
std::optional<T const&> BDIndexMap::val( size_t n ) {

    if( n >= data.size() )
        return std::nullopt;

    return data[n];
}

} // namespace util
