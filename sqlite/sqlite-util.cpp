/****************************************************************
* SQLite Utilities
****************************************************************/
#include "sqlite-util.hpp"

using namespace std;

namespace sqlite {

namespace {

// Function that will throw if a path does not exist, but only if
// the object requires it.
void dbdesc_exists( DBDesc const& d ) {
    // If it is not required to exist then just return.
    if( !get<bool>( d ) )
        return;

    auto const& path = get<fs::path>( d );
    ASSERT( fs::exists( path ),
            "database file " << path << " does not exist." );
}

} // anonymous namespace

// We need to provide this because, if  not,  then  the  compiler
// will attempt to automatically convert the  path  to  a  string
// (because the sqlite wrapper has no overload for  paths)  which
// will  fail  on windows because on Windows the automatic string
// conversion  from  a  path is to a wide string (unlike on Linux,
// where it would work). So what we  have  to do here is to force
// the path to a standard string. Also, note return type.
sqlite::database_binder& operator<<( sqlite::database_binder& db,
                                     fs::path const& path ) {
    // The  below  string conversion will always be to a standard
    // (i.e., non-wide) string on all  platforms and will not sur-
    // round  the  string  with quotes (unlike fs::path's default
    // streaming operator).
    return (db << path.string());
}

// Attach to an existing connection.
void attach( sqlite::database& db, DBDescVec const& dbs ) {

    for( auto const& desc : dbs ) {
        auto const& [path, name, exists] = desc;
        (void)exists;
        dbdesc_exists( desc );
        db << "ATTACH DATABASE ? as ?" << path.string() << name;
    }
}

// Open  a  primary database connection and, optionally, a series
// of additional ones. Note that the alias name specified in  the
// primary  database  connection  is  ignored (it will not be put
// under an alias), however any others in `rest` will. Also, this
// will run some initialization on the database and so it is  rec-
// ommended to  always  open  the  databases  using  this  method.
sqlite::database open( DBDesc    const& primary,
                       DBDescVec const& rest ) {

    dbdesc_exists( primary );
    sqlite::database db( get<fs::path>( primary ).string() );

    attach( db, rest );

    // sqlite  disables  this  by default, so we need to manually
    // enable it whenever we open  a  connection. We enable it au-
    // tomatically  because  a)  we  use it, abd b) there doesn't
    // seem to be any good reason to have it disabled.
    db << "PRAGMA foreign_keys = ON";

    return db;
}

} // namespace sqlite
