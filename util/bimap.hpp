/****************************************************************
* Bi-directional Map Classes
****************************************************************/
#pragma once

#include "non-copyable.hpp"
#include "util.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <vector>

namespace util {

/****************************************************************
* BDIndexMap
*
* This class will map a collection of unique items of  the  speci-
* fied  type  to  a unique list of integers. Mapping from key (al-
* ways integer) to value  (specified  type)  will  happen in O(1)
* time. Mapping from value to key will be O(ln(N)) time.
*
* Values are returned as optional references.
****************************************************************/
template<typename T> class BDIndexMap : util::non_copyable {

public:
    // Use reference_wrapper to hold a reference in a container.
    using OptRef = std::optional<std::reference_wrapper<T const>>;

    // NOTE: the contained data must be a vector of unique items;
    // if what you are passing in does not meet that  requirement
    // then  set  the is_uniq_sorted flag to false and it will be
    // done for you. If you don't do this then this class may not
    // function properly.
    BDIndexMap( std::vector<T>&& data,
                bool             is_uniq_sorted = false );

    // Returns #keys (== #values)
    size_t size() const { return m_data.size(); }

    // Returns an optional  of  reference,  so  no copying/moving
    // should happen here.
    OptRef val( size_t n ) const;

    std::optional<size_t> key( T const& val ) const;

private:

    std::vector<T> m_data;
};

template<typename T>
BDIndexMap<T>::BDIndexMap( std::vector<T>&& data,
                           bool is_uniq_sorted )
    : m_data( move( data ) ) {

    if( !is_uniq_sorted )
        util::uniq_sort( m_data );
}

template<typename T>
std::optional<size_t> BDIndexMap<T>::key( T const& val ) const {

    auto i = std::lower_bound(
                begin( m_data ), end( m_data ), val );

    if( i != end( m_data ) && *i == val )
        return i - begin( m_data );

    return std::nullopt;
}

template<typename T>
typename BDIndexMap<T>::OptRef
BDIndexMap<T>::val( size_t n ) const {

    if( n >= m_data.size() )
        return std::nullopt;

    return m_data[n];
}

} // namespace util
