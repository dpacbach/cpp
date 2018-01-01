/****************************************************************
* SQLite Utilities
****************************************************************/
#pragma once

#include "sqlite_modern_cpp.h"
#include "string-util.hpp"

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

namespace sqlite {

namespace impl {

// This function exists for the purpose of  having  the  compiler
// deduce the Indexes variadic integer arguments that we can then
// use to index the tuple.  Note  that this function expects that
// the  query  string will already have been sent to the database.
// Also, the object that recieves the arguments is  left  generic
// so that it can be  either  a  database or a prepared statement.
template<typename Receiver, typename Tuple, size_t... Indexes>
void insert_tuple_impl( Receiver&    db,
                        Tuple const& t,
                        std::index_sequence<Indexes...> ) {

    // Unary right fold of template parameter pack.
    ((db << std::get<Indexes>( t )), ...);
}

}

// Take a query with parameters and a tuple of arguments and will
// run  the query and feed in the components of the tuple individ-
// ually for substitution. In order to  unpack  the  tuple  in  a
// generic way it seems that we have  to  use  the  above  helper
// function involving index sequences unfortunately.
template<typename... Args>
void insert_tuple( sqlite::database&  db,
                   std::string const& query,
                   std::tuple<Args...> const& t ) {

    db << query;
    auto is = std::make_index_sequence<sizeof...(Args)>();
    impl::insert_tuple_impl( db, t, is );
}

// Just iterate through the vector and  add in each tuple individ-
// ually. Despite the fact that we are using a prepared statement
// (and only streaming the query string  once) it seems that this
// is  still  extremely slow, so should probably not be used when
// there are more than a handful of elements.
template<typename... Args>
void insert_many( sqlite::database&  db,
                  std::string const& query,
                  std::vector<std::tuple<Args...>> const& in ) {

    auto is = std::make_index_sequence<sizeof...(Args)>();
    // Get "prepared statement" which is not immediately executed.
    auto ps = (db << query);

    for( auto const t : in ) {
        impl::insert_tuple_impl( ps, t, is );
        ps.execute();
    }
}

// Takes  a  partial  query  without the values and will manually
// convert the tuples to strings and insert them into  the  query
// in a list to improve insertion time significantly.
template<typename... Args>
void insert_many_fast( sqlite::database&  db,
                       std::string const& query,
                       std::vector<std::tuple<Args...>> const& in ) {

    using Tp = std::tuple<Args...>;

    std::vector<std::string> strs( in.size() );
    // We  need  this lambda to help std::transform with overload
    // resolution of to_string.
    auto f = []( Tp const& e ){ return util::to_string( e ); };
    std::transform( std::begin( in   ), std::end( in ),
                    std::begin( strs ), f );

    // Insert all elements in one shot.
    db << (query + " " + util::join( strs, "," ));
}

} // namespace sqlite
