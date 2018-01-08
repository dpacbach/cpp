/****************************************************************
* SQLite Utilities
****************************************************************/
#pragma once

#include "fs.hpp"
#include "sqlite_modern_cpp.h"
#include "string-util.hpp"

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

namespace sqlite {

namespace impl {

// When building large queries  with  many  substituted values we
// will do it in chunks for this many elements in order to  avoid
// generating query strings that exceed sqlite's  maximum  length.
size_t constexpr chunk = 2000;

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

// We use this to extract  the  message from the sqlite_exception
// if we catch one  since  there  is  some additional information
// (namely, the offending SQL query) which is not included in the
// default message.
std::string exception_msg( sqlite_exception const& e );

// We need to provide this because, if  not,  then  the  compiler
// will attempt to automatically convert the  path  to  a  string
// (because the sqlite wrapper has no overload for  paths)  which
// will  fail  on windows because on Windows the automatic string
// conversion  from  a  path is to a wide string (unlike on Linux,
// where it would work). So what we  have  to do here is to force
// the path to a standard string.
sqlite::database_binder& operator<<( sqlite::database_binder& db,
                                     fs::path const& path );

// This  is a pair used to hold information about additional data-
// bases  that should be added into the connection beyond the pri-
// mary. The first element is the file path to the database,  the
// second is the name that it will be given for the  purposes  of
// accessing it from within  queries,  and  the  third  indicates
// whether it is required to exist.
using DBDesc    = std::tuple<fs::path, std::string, bool>;
using DBDescVec = std::vector<DBDesc>;

// Open  a  primary database connection and, optionally, a series
// of additional ones. Note that the alias name specified in  the
// primary  database  connection  is  ignored (it will not be put
// under an alias), however any others in `rest` will. Also, this
// will run some initialization on the database and so it is  rec-
// ommended to  always  open  the  databases  using  this  method.
sqlite::database open( DBDesc    const& primary,
                       DBDescVec const& rest = {} );

// Attach to an existing connection.
void attach( sqlite::database& db, DBDescVec const& dbs );

// With move and/or NRVO this should be  a  convenient  yet  effi-
// cient way to select data from the  db.  This  variant  is  for
// queries that return multiple columns.
template<typename... Args>
std::vector<std::tuple<Args...>>
select( sqlite::database& db, std::string const& query ) {

    std::vector<std::tuple<Args...>> res;

    auto func = [&res]( Args&&... args ) {
        res.emplace_back( std::move( args )... );
    };

    // For each resulting row run  the  above function which will
    // append it to the list.
    db << query >> func;

    return res;
}

// With move and/or NRVO this should be  a  convenient  yet  effi-
// cient way to select data from the  db.  This  variant  is  for
// queries that return a single column.
template<typename T>
std::vector<T> select1( sqlite::database&  db,
                        std::string const& query ) {

    std::vector<T> res;

    auto func = [&res]( T&& arg ) {
        res.emplace_back( std::move( arg ) );
    };

    // For each resulting row run  the  above function which will
    // append it to the list.
    db << query >> func;

    return res;
}

// Not sure exactly what this does, but  it  seems  like  a  good
// thing to do from time to  time  on  a database when editing it.
inline void vacuum( sqlite::database& db ) { db << "VACUUM"; }

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

// This overload is for when  the  query  string has multiple sub-
// stitution  parameters. Just iterate through the vector and add
// in each tuple individually. Despite the fact that we are using
// a prepared statement  (and  only  streaming  the  query string
// once)  it  seems  that this is still extremely slow, so should
// probably  not be used when there are more than a handful of el-
// ements.
template<typename... Args>
void insert_many( sqlite::database&  db,
                  std::string const& query,
                  std::vector<std::tuple<Args...>> const& in ) {

    auto is = std::make_index_sequence<sizeof...(Args)>();
    // Get "prepared statement" which is not immediately executed.
    auto ps = (db << query);

    for( auto const& t : in ) {
        impl::insert_tuple_impl( ps, t, is );
        ps.execute();
    }
}

// This  overload  is for when the query string has only a single
// substitution parameter. Just  iterate  through  the vector and
// add in each element individually. Despite the fact that we are
// using  a  prepared  statement  (and  only  streaming the query
// string once) it seems that  this  is  still extremely slow, so
// should probably not be used when there are more than a handful
// of elements.
template<typename T>
void insert_many( sqlite::database&     db,
                  std::string const&    query,
                  std::vector<T> const& in ) {

    // Get "prepared statement" which is not immediately executed.
    auto ps = (db << query);

    for( auto const& t : in ) {
        ps << t;
        ps.execute();
    }
}

// This overload is for when  the  query  string has multiple sub-
// stitution parameters. Takes a partial query without the values
// and  will  manually  convert  the tuples to strings and insert
// them  into  the  query in a list to improve insertion time sig-
// nificantly. NOTE: this will only work for queries that end  in
// a list of tuples containing only  argument  substitutions,  as
// opposed to queries whose  substitution  marks  are embedded in
// the query in a more complicated way.
template<typename... Args>
void insert_many_fast( sqlite::database&  db,
                       std::string const& query,
                       std::vector<std::tuple<Args...>> const& in ) {

    using Tp = std::tuple<Args...>;

    // We  need  this lambda to help std::transform with overload
    // resolution of to_string.
    auto f = []( Tp const& e ){ return util::to_string( e ); };

    // Do  the  entire  operation  in  chunks  to avoid exceeding
    // sqlite's maximum query length.
    for( auto [l,r] : util::chunks( in.size(), impl::chunk ) ) {
        // This is the number  of  elements  in this chunk, which
        // may  be less than impl::chunk if we're on the last one.
        std::vector<std::string> strs( r-l );
        // l,r are offsets from beginning of vector, and r  is  a
        // one-past-the-end offset.
        std::transform( std::begin( in )+l, std::begin( in )+r,
                        std::begin( strs ), f );

        // Insert all elements in one shot.
        db << (query + " " + util::join( strs, "," ));
    }
}

// This overload is for  the  general  case  when  the caller can
// specify a function to convert the vector element into a string
// that can then be inserted as a value-tuple in the query
// string.  The  function  should  have  the following signature:
//
//   std::string( T const& )
//
// and it should convert that single element to  a  string  repre-
// senting a tuple, which  should  be surrounded with parenthesis
// (i.e., "(5)", or "(5, 'hello')", etc. Note that even a 1-tuple
// needs to have the  parenthesis.  This  function will be called
// for  every element in the input vector and the results will be
// joined with commas and then put at the end of the query
// string.
//
// NOTE: this will only work for  queries  that  end in a list of
// tuples containing only argument  substitutions,  as opposed to
// queries whose substitution marks are embedded in the query  in
// a more complicated way.
template<typename T, typename Func>
void insert_many_fast( sqlite::database&     db,
                       std::string const&    query,
                       std::vector<T> const& in,
                       Func                  func ) {

    // Do  the  entire  operation  in  chunks  to avoid exceeding
    // sqlite's maximum query length.
    for( auto [l,r] : util::chunks( in.size(), impl::chunk ) ) {
        // This is the number  of  elements  in this chunk, which
        // may  be less than impl::chunk if we're on the last one.
        std::vector<std::string> strs( r-l );
        // l,r are offsets from beginning of vector, and r  is  a
        // one-past-the-end offset.
        std::transform( std::begin( in )+l, std::begin( in )+r,
                        std::begin( strs ), func );

        // Insert all elements in one shot.
        db << (query + " " + util::join( strs, "," ));
    }
}

// This is a variant of the above for the simple  case  that  the
// type T represents a single element  (not  a tuple) and that we
// want to simply call util::to_string on it.
template<typename T>
void insert_many_fast( sqlite::database&     db,
                       std::string const&    query,
                       std::vector<T> const& in ) {
    auto f = L( "(" + util::to_string( _ ) + ")" );
    insert_many_fast( db, query, in, f );
}

} // namespace sqlite
