/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "fs.hpp"
#include "logger.hpp"
#include "main.hpp"
#include "sqlite-util.hpp"
#include "string-util.hpp"
#include "stopwatch.hpp"

#include <string>
#include <iostream>
#include <vector>

using namespace std;
using util::operator<<;

int main_( int, char** )
{
    util::Logger::enabled = true;

    auto db_name = "../test.db";

    if( fs::exists( db_name ) )
        fs::remove( db_name );

    sqlite::database db( db_name );

    db << "CREATE TABLE IF NOT EXISTS user ("
          "   _id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
          "   age INT,"
          "   name TEXT,"
          "   weight REAL,"
          "   data BLOB"
          ");";

    // Test inserting many rows from vector  from  a  single  pre-
    // pared statement.
    vector<tuple<int, string, double>> input_rows{
        {54, "rich", 45.0},
        {43, "mark", 56.0},
        {32, "ted",  67.0},
        {87, "dave", 78.0}
    };

    vector<tuple<int, string, double>> rows;

    int num_rows = 4'000'000;

    util::timeit( "insert into vector", [&]{
        for( int i = 0; i < num_rows; ++i )
            rows.push_back( input_rows[i % 4] );
    } );

    TIMEIT( "do insertion of " + util::to_string( num_rows) + " rows",
        // Prepare the query once,  then  execute  once  for each row.
        sqlite::insert_many_fast( db,
            "INSERT INTO user (age, name, weight) VALUES",
            rows
        )
    );

    util::log << "\n";
    util::log << "finished.\n";

    return 0;
}
