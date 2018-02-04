/****************************************************************
* SQLite Utilities
****************************************************************/
#pragma once

#include "datetime.hpp"
#include "fs.hpp"
#include "sqlite_modern_cpp.h"
#include "string-util.hpp"
#include "types.hpp"

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

// This is the top-level (unspecialized) to_string method that we
// use  in  this  module when we need to manually convert a query
// substitution  argument  to  a string. We may want to do manual
// conversion (instead of letting our sqlite cpp  wrapper  do  it
// for us) in order  to  manually  construct  queries that insert
// many values in a single shot. Furthermore, the reason we don't
// use  util::to_string  to do the conversion is that we need spe-
// cialized behavior for a few types (and  tuples  thereof).  For
// example, just like the sqlite_modern_cpp  wrapper, we want our
// API to support using std::optional types to  represent  column
// that can be null, in which case std::nullopt represents a null
// values, which is then converted to  NULL in the query (with no
// quotes). Important: we must use these  impl::to_string  family
// of  functions (defined in this module) whenever we need to con-
// vert a query parameter to a string in this module.
//
// In  what  follows  we  create  just  enough specializations of
// to_string  to support all of the to_string conversions that we
// need to handle differently than the default implementation  in
// util::to_string.
template<typename T>
std::string to_string( T const& what ) {
    // Default case, just delegate.
    return util::to_string( what );
}

// We  need  special  SQL-specifc  behavior for the std::optional.
template<typename T>
std::string to_string( std::optional<T> const& what ) {
    if( !what )
        return "NULL";
    return util::to_string( *what );
}

// We  need  special  SQL-specifc behavior for the time points in
// that  we  need them surrounded in quotes because we will store
// them in the sqlite database as strings.
template<>
std::string to_string( SysTimePoint const& p );
template<>
std::string to_string( ZonedTimePoint const& p );

// This function exists for the purpose of  having  the  compiler
// deduce the Indexes variadic integer arguments that we can then
// use to index the tuple; it probably is not useful to call this
// method directly (it is called by to_string).
template<typename Tuple, size_t... Indexes>
StrVec tuple_elems_to_string( Tuple const& tp,
                              std::index_sequence<Indexes...> ) {
    StrVec res; res.reserve( std::tuple_size_v<Tuple> );
    namespace N = ::sqlite::impl;
    // Unary  right  fold  of  template parameter pack. NOTE: the
    // to_string  method  used  here  is  the  one in this module.
    ((res.push_back( N::to_string( std::get<Indexes>( tp ) ))), ...);
    return res;
}

// Will do JSON-like notation.  E.g.  (1,"hello",2),  however  it
// calls impl::to_string (in this module) so that components that
// are optional will be properly converted  to "NULL" if they are
// nullopt.
template<typename... Args>
std::string to_string( std::tuple<Args...> const& tp ) {
    auto is = std::make_index_sequence<sizeof...(Args)>();
    auto v = ::sqlite::impl::tuple_elems_to_string( tp, is );
    return "(" + util::join( v, "," ) + ")";
}

// This is a helper function that accepts a tuple as an  (unused)
// argument  to  allow  deduction of the tuple's field types into
// the function template's variadic args. Those variadic args are
// then used to construct a  lambda  function that takes one para-
// meter for each of the tuple types and attempts to construct an
// object of type T from  them  using brace initialization syntax.
// Ultimately, this function  serves  the  end  goal of selecting
// multiple  columns  from a database and constructing objects of
// user-defined structs from them.
template<typename T, typename... Args>
std::vector<T> select_struct( sqlite::database_binder&& db,
                              std::tuple<Args...> const& ) {

    std::vector<T> res;

    db >> [&]( Args&&... args ) {
        // Using this emplace_back +  brace  initialization + RVO
        // we are hoping to avoid any copying or moving.
        res.emplace_back( T{ std::move( args )... } );
    };

    return res;
}

} // namespace impl

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

// This allows us to insert local or zoned time points  which  we
// do  by  just converting them to a string in a standard format:
// 2018-01-15 21:30:01.396823389[-0000] (local time will have not
// time zone at the end, while zoned time will always be  in  UTC
// with a +0000 at the  end).  We  do this because SQLite doesn't
// really have a proper date/time data type. Note that strings in
// this format are useful because  their  time ordering can be de-
// termined by lexicographically comparing  their string represen-
// tations (but only if time zones are the same).
sqlite::database_binder& operator<<( sqlite::database_binder& db,
                                     SysTimePoint const& p );
sqlite::database_binder& operator<<( sqlite::database_binder& db,
                                     ZonedTimePoint const& p );

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

// This  function  is  used  when one wants to select rows from a
// database  and  convert the contents of each row into an object
// of a user-defined class instead  of  e.g. returning a tuple of
// columns separately. The requirements for this to work are that
// a) the class or struct T must have a type member  `tuple_type`
// that  supplies  the  type  list of fields of the struct in the
// form of a tuple (because  it  seems that with template metapro-
// gramming alone, the C++ language does not allow  deducing  the
// types of the fields of a  class),  and  b) the class or struct
// needs  to  be  initializable using brace initialization syntax
// when given a list of fields of the correct type.  For  example,
// to make the `Complex`  struct  usable  with this function, one
// would simply add the tuple_type as follows:
//
// struct Complex {
//
//     using tuple_type = std::tuple<double, double>;
//
//     double real;
//     double imag;
// };
//
// Then one could call:
//
//     vector<Complex> res = select_struct<Complex>( db,
//         "SELECT real, imag FROM ComplexNumbers" );
//
// One  disadvantage  with  this  approach is that the tuple_type
// must be kept in sync manually with the fields  of  the  struct.
// This is prone to error,  however  it  turns out that this func-
// tion is actually pretty good at triggering either  compile  er-
// rors or at least warning when there is any  kind  of  mismatch
// between the two. Unfortunately though, an error  that  is  not
// properly caught is when a  new  field  is  added to the struct
// (and  tuple  type)  but  the SQL queries are not updated to in-
// clude  that  extra  field. The sqlite C++ wrapper we are using
// will simply  supply  that  parameter  as  default  constructed.
template<typename T>
std::vector<T> select_struct( sqlite::database&  db,
                              std::string const& query ) {

    // The type T is required to have a type field tuple_type. We
    // don't care about this tuple  per  se,  we just need to get
    // the types of its elements, which are assumed to correspond
    // to the types of the fields of  the  struct  (no  more,  no
    // less, and in order).
    using fields_t = typename T::tuple_type;
    return impl::select_struct<T>( db << query, fields_t{} );
}

// This  macro  should be called just after the definition of any
// class  that will be used with the select_struct function above,
// i.e., one that has a tuple_type member. It will check that the
// struct has that member and  will  also  check that the size of
// the struct matches the size of the tuple type exactly in order
// to catch situations where a field is added or removed from the
// struct  by the tuple_type is not updated accordingly. The fact
// that  these size should agree is probably implementation depen-
// dant and so it may not be  good  practice to rely on this, but
// it seems reasonable and it works with gcc.
//
// It seems that if the struct T  is  missing a field or if there
// is a type mismatch between a field and the  corresponding  one
// in  the  tuple_type  that the compiler would give an error any-
// way;  however, there are cases (such as when the type T simply
// has an extra field not present in the tuple) that the compiler
// would only give a warning, and so this  will  ensure  that  it
// triggers a compile error.
#define CHECK_TUPLE_SIZE( T )                                   \
    static_assert( sizeof( T ) == sizeof( T::tuple_type ),      \
                  "fields of " #T "'s tuple_type must be kept " \
                  "consistent with the number, order, and "     \
                  "types of " #T " itself." );

// With move and/or NRVO this should be  a  convenient  yet  effi-
// cient way to select data from the  db.  This  variant  is  for
// queries that return  multiple  columns.  This  variant takes a
// database  binder  for  a  query that has already been prepared
// (perhaps  with  some  substitutions).  We  take  the binder by
// rvalue reference because 1) we're  going  to execute it (after
// which it won't be of any value), and 2) so that the caller can
// pass  a temporary binder object to this function which is some-
// times convenient.
template<typename... Args>
std::vector<std::tuple<Args...>>
select( sqlite::database_binder&& db ) {

    std::vector<std::tuple<Args...>> res;

    auto func = [&res]( Args&&... args ) {
        res.emplace_back( std::move( args )... );
    };

    // For each resulting row run  the  above function which will
    // append it to the list.
    db >> func;

    return res;
}

// Same as above but takes a query  string  ready  for  execution.
template<typename... Args>
std::vector<std::tuple<Args...>>
select( sqlite::database& db, std::string const& query ) {

    return select<Args...>( db << query );
}

// With move and/or NRVO this should be  a  convenient  yet  effi-
// cient way to select data from the  db.  This  variant  is  for
// queries that return  a  single  column.  This  variant takes a
// database  binder  for  a  query that has already been prepared
// (perhaps  with  some  substitutions).  We  take  the binder by
// rvalue reference because 1) we're  going  to execute it (after
// which it won't be of any value), and 2) so that the caller can
// pass  a temporary binder object to this function which is some-
// times convenient.
template<typename T>
std::vector<T> select1( sqlite::database_binder&& db ) {

    std::vector<T> res;

    auto func = [&res]( T&& arg ) {
        res.emplace_back( std::move( arg ) );
    };

    // For each resulting row run  the  above function which will
    // append it to the list.
    db >> func;

    return res;
}

// Same as above but takes a query  string  ready  for  execution.
template<typename T>
std::vector<T> select1( sqlite::database&  db,
                        std::string const& query ) {

    return select1<T>( db << query );
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
                  std::vector<std::tuple<Args...>> const& in,
                  std::string const& query ) {

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
                  std::vector<T> const& in,
                  std::string const&    query ) {

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
                       std::vector<std::tuple<Args...>> const& in,
                       std::string const& query ) {

    using Tp = std::tuple<Args...>;

    // We  need  this lambda to help std::transform with overload
    // resolution of to_string.
    auto f = []( Tp const& e ){ return impl::to_string( e ); };

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
                       std::vector<T> const& in,
                       std::string const&    query,
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
                       std::vector<T> const& in,
                       std::string const&    query ) {
    auto f = L( "(" + impl::to_string( _ ) + ")" );
    insert_many_fast( db, in, query, f );
}

} // namespace sqlite
