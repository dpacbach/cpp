/****************************************************************
* Unit tests
****************************************************************/
#include "common-test.hpp"

#include "opt-util.hpp"
#include "sqlite-util.hpp"
#include "string-util.hpp"
#include "util.hpp"

using namespace std;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( sqlite )
{
    // This  will  create  a  database in memory so that we don't
    // have to bother with temp files.
    sqlite::database db = sqlite::open( { ":memory:", "", false } );

    db << "CREATE TABLE IF NOT EXISTS user ("
          "   _id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
          "   age INT,"
          "   name TEXT,"
          "   weight REAL,"
          "   data BLOB"
          ");";

    db << "INSERT INTO USER (age,name,weight) VALUES (?,?,?);"
       << 18
       << u"bob"
       << 83.25;

    int    age    = 21;
    double weight = 68.5;
    string name   = "jack";

    db << u"INSERT INTO USER (age,name,weight) VALUES (?,?,?);"
       << age
       << name
       << weight;

    EQUALS( db.last_insert_rowid(), 2 );

    // Test that we get nothing if  we  select a row id that does
    // not exist.
    db << "SELECT age, name, weight FROM user WHERE _id=3;"
       >> []( int, string const&, double ){
              TRUE( false, "got row but there should be none" );
          };

    // Selects from user table on a  condition (age > 18) and exe-
    // cutes the lambda for each row returned.
    db << "SELECT age, name, weight FROM user WHERE age > ? ;"
       << 18
       >> []( int age, string name, double weight ){
          EQUALS( age,     21    );
          EQUALS( name,   "jack" );
          EQUALS( weight,  68.5  );
       };

    int count = 0;
    db << "SELECT COUNT(*) FROM user" >> count;
    EQUALS( count, 2 );

    // Test SQL syntax error.
    THROWS( db << "SELECTx COUNT(*) FROM user" >> count );

    // Test unpacking to wrong number of params.
    // db << "SELECT COUNT(*), COUNT(*) FROM user" >> count;
    // Note: above line doesn't throw... library defect?

    // Test unpacking to wrong number of params.
    THROWS( db << "SELECT * FROM user" >> count );
    // Test inputting wrong number of params.
    THROWS( db << "" << 5 );

    db << "SELECT age, name FROM user WHERE _id=1;"
       >> tie( age, name );
    EQUALS( age,   18   );
    EQUALS( name, "bob" );

    // Test automatic string conversion.
    db << "SELECT COUNT(*) FROM user" >> []( string const& s ){
        EQUALS( s, "2" );
    };

    db << "INSERT INTO USER (age,name,weight) VALUES (?,NULL,?);"
       << 30
       << 100.0;

    EQUALS( db.last_insert_rowid(), 3 );
    db << "SELECT COUNT(*) FROM user" >> []( int c ){
        EQUALS( c, 3 );
    };

    // The following function signature should throw if we try to
    // extract row 3 int it since name is null,  but  instead  it
    // seems  the  library  will  just default construct the para-
    // meter in these cases, which is bad...
    // []( int age, string const& name, double weight )

    // Test optional
    auto opt = []( int age, OptStr const& name, double weight ){
        EQUALS( age,    30 );
        EQUALS( name,   nullopt );
        EQUALS( weight, 100.0 );
    };
    string query =
        "SELECT age, name, weight FROM user WHERE _id=3;";

    db << query >> opt;

    // Test variant. Below  function  body  should  not be called.
    auto var = []( int, variant<string> const&, double ){};
    // This  should throw because the variant (with no nullptr_t)
    // type should not allow null values.
    THROWS( db << query >> var );

    // Test blobs.
    db << "INSERT INTO user (name,data) VALUES (?,?);"
       << "fred"
       << vector<char>{ 'a', 'c', 'x' };
    EQUALS( db.last_insert_rowid(), 4 );

    vector<char> v;
    db << "SELECT data FROM user WHERE _id=4" >> v;
    EQUALS( v, (vector<char>{ 'a', 'c', 'x' }) );

    // Test inserting many rows from vector  from  a  single  pre-
    // pared statement.
    vector<tuple<int, string, double>> rows{
        {54, "rich", 45.0},
        {43, "mark", 56.0},
        {32, "ted",  67.0},
        {87, "dave", 78.0}
    };

    // Prepare the query once,  then  execute  once  for each row.
    sqlite::insert_many( db, rows,
        "INSERT INTO user (age, name, weight) VALUES (?,?,?)"
    );

    // Spot-check a row.
    db << "SELECT age, name, weight FROM user WHERE _id=7 ;"
       >> []( int age, string name, double weight ){
          EQUALS( age,     32   );
          EQUALS( name,   "ted" );
          EQUALS( weight,  67.0 );
       };

    // Run  same  test  again,  but this time with the "fast" ver-
    // sion.
    sqlite::insert_many_fast( db, rows,
        "INSERT INTO user (age, name, weight) VALUES"
    );

    // Spot-check a row.
    db << "SELECT age, name, weight FROM user WHERE _id=11 ;"
       >> []( int age, string name, double weight ){
          EQUALS( age,     32   );
          EQUALS( name,   "ted" );
          EQUALS( weight,  67.0 );
       };

    // Test inserting many rows from vector  from  a  single  pre-
    // pared statement without a tuple inside (we have a special
    // overload for this).
    vector<int> rows_1{ 55, 44, 33, 88 };

    // Prepare the query once,  then  execute  once  for each row.
    sqlite::insert_many( db, rows_1,
        "INSERT INTO user (age) VALUES (?)"
    );

    // Spot-check a row.
    db << "SELECT age, name, weight FROM user WHERE _id=15 ;"
       >> []( int age, string name, double weight ){
          EQUALS( age,     33 );
          EQUALS( name,    "" );
          EQUALS( weight,  0  );
       };

    // Run  same  test  again,  but this time with the "fast" ver-
    // sion.
    sqlite::insert_many_fast( db, rows_1,
        "INSERT INTO user (age) VALUES"
    );

    // Spot-check a row.
    db << "SELECT age, name, weight FROM user WHERE _id=19 ;"
       >> []( int age, string name, double weight ){
          EQUALS( age,     33 );
          EQUALS( name,    "" );
          EQUALS( weight,  0  );
       };

    // Now test chunking; we insert a  number of rows a few times
    // larger than the chunk size to make sure we  have  multiple
    // chunks.
    vector<tuple<int, string>> rows2;
    for( size_t i = 0; i < sqlite::impl::chunk*3+7; ++i )
        rows2.push_back( { 333, util::to_string( i ) } );

    sqlite::insert_many_fast( db, rows2,
        "INSERT INTO user (age, name) VALUES"
    );

    // Check all the rows.
    size_t count2 = 0;
    db << "SELECT age, name FROM user "
          "WHERE age=333 ORDER BY _id ASC"
       >> [&count2]( int, string name ){
          EQUALS( name, util::to_string( count2 ) );
          ++count2;
       };

    // Now do a query with the select function.
    using res_type_2 = vector<tuple<int, string>>;
    auto v2 = sqlite::select<int, string>( db,
        "SELECT age, name FROM user WHERE age=333 "
        "ORDER BY _id ASC LIMIT 5"
    );

    EQUALS( v2, (res_type_2{ {333,"0"},{333,"1"},{333,"2"},
                             {333,"3"},{333,"4"} }) );

    using res_type_3 = vector<string>;
    auto v3 = sqlite::select1<string>( db,
        "SELECT name FROM user WHERE age=333 "
        "ORDER BY _id ASC LIMIT 5"
    );

    EQUALS( v3, (res_type_3{ "0","1","2","3","4" }) );

    // Test inserting fs::path's into  string columns. In particu-
    // lar,  we are testing that the path gets properly converted
    // to  a  string  without  quotes around it, which the util::-
    // to_string method would do (as well as fs::path's streaming
    // operator<<);  i.e.,  when we insert a path into the DB, we
    // do not want quotes around  it.  By default  the path would
    // just  get cast to a string which would fail on  Windows be-
    // cause there paths  by default are  represented by wide str-
    // ing, so we have provided  our own overload of the database
    // binder streaming operator which will call path.string() to
    // force it  to a standard string.  If the below compiles and
    // passes then that mechanism is working.
    db << "INSERT INTO user (age, name) VALUES (?, ?)"
       << 765 << fs::path( "A/B/C" );

    auto v4 = sqlite::select<int, string>( db,
        "SELECT age, name FROM user WHERE age=765"
    );
    EQUALS( v4.size(), 1 );
    EQUALS( get<string>( v4[0] ), "A/B/C" );

    // Test that, on error, the library throws a sqlite_exception
    // and that the query appears  in  the  error message that we
    // get by calling exception_msg() on it.
    bool threw = true, good_except = false, good_msg = false;
    string q = "SELECTx * from user";
    try {
        db << q;
        // If we're here then we have a problem because the above
        // query is supposed to throw. However,  we  can't  throw
        // here  to  indicate  test  failure otherwise it will be
        // caught below.
        threw = false;
    } catch( sqlite::sqlite_exception const& e ) {
        good_except = true;
        auto msg = sqlite::exception_msg( e );
        if( util::contains( msg, "SELECTx" ) )
            good_msg = true;
    } catch( ... ) {

    }
    // Test to make sure the query  threw  an  exception  at  all.
    TRUE( threw, "query " << quoted( q ) << " did not throw" );
    // Make sure it  through  an  exception  of  the  right  type.
    TRUE( good_except, "query " << quoted( q ) << " threw an "
            " exception but it was not a sqlite_exception" );
    // Make sure the error  message  returned  by  our  error  ex-
    // tracting function included the sql query.
    TRUE( good_msg, "query " << quoted( q ) << " threw a "
            "sqlite_exception but the exception_msg function "
            "did not yield the query in its message." );

    // Test the insert_many_fast with custom function.
    vector<tuple<int, int, int>> tp3{
        { 0, 299, 1 },
        { 0, 299, 1 },
        { 0, 299, 1 },
    };
    sqlite::insert_many_fast( db, tp3,
        "INSERT INTO user (age, weight) VALUES", LC(
        string( "(" ) + util::to_string( get<1>( _ ) ) +
        ", " + util::to_string( 8.9 ) + ")"
    ) );

    int c; double w;
    db << "SELECT COUNT(*) FROM user WHERE age=299"        >> c;
    db << "SELECT DISTINCT weight FROM user WHERE age=299" >> w;
    EQUALS( c, 3   );
    EQUALS( w, 8.9 );

    // Test that we can send and  receive  NULL  values  properly
    // using std::optional and using  the  insert_many_fast  func-
    // tions.
    vector<tuple<optional<int>, OptStr, double>> rows3{
        { 654,     "hello",  7.6 },
        { 654,      nullopt, 7.6 },
        { nullopt,  nullopt, 7.6 }
    };

    sqlite::insert_many_fast( db, rows3,
        "INSERT INTO user (age, name, weight) VALUES" );

    auto v5 = sqlite::select<optional<int>, OptStr>( db,
        "SELECT age, name FROM user WHERE weight=7.6" );

    EQUALS( v5.size(), 3 );

    EQUALS( std::get<0>( v5[0] ), 654     );
    EQUALS( std::get<0>( v5[1] ), 654     );
    EQUALS( std::get<0>( v5[2] ), nullopt );
    EQUALS( std::get<1>( v5[0] ), "hello" );
    EQUALS( std::get<1>( v5[1] ), nullopt );
    EQUALS( std::get<1>( v5[2] ), nullopt );

    auto now = chrono::system_clock::now();

    db << "INSERT INTO user (age, name) VALUES (?, ?)"
       << 987 << now;

    vector<tuple<int, SysTimePoint>> rows4{ { 987, now } };

    sqlite::insert_many_fast( db, rows4,
        "INSERT INTO user (age, name) VALUES" );

    auto v6 = sqlite::select1<string>( db,
        "SELECT name FROM user WHERE age=987" );

    EQUALS( v6.size(), 2 );
    EQUALS( v6[0].size(), 29 );
    EQUALS( v6[1].size(), 29 );

    ZonedTimePoint now_zoned( now, util::tz_utc() );

    db << "INSERT INTO user (age, name) VALUES (?, ?)"
       << 988 << now_zoned;

    vector<tuple<int, ZonedTimePoint>> rows5{ { 988, now_zoned } };

    sqlite::insert_many_fast( db, rows5,
        "INSERT INTO user (age, name) VALUES" );

    auto v7 = sqlite::select1<string>( db,
        "SELECT name FROM user WHERE age=988" );

    EQUALS( v7.size(), 2 );
    EQUALS( v7[0].size(), 34 );
    EQUALS( v7[1].size(), 34 );
}

TEST( select_struct )
{
    struct S {
        using tuple_type = tuple<int, int, string, double>;

        int    a;
        int    b;
        string c;
        double d;
    };

    // If this macro fails to compile and you  have  not  changed
    // anything about the struct S above, then  it  could  be  an
    // implementation-specific  thing  where  tuples  and structs
    // with same fields are not the same size.
    CHECK_TUPLE_SIZE( S )

    auto db = sqlite::open( { ":memory:", "", false } );

    db << "CREATE TABLE t (a int, b int, c text, d float)";

    db << "INSERT INTO t (a, b, c, d) values (4, 3, 'hello', 2.3)";
    db << "INSERT INTO t (a, b, c, d) values (3, 4, 'hxllo', 2.1)";

    auto vs = sqlite::select_struct<S>( db, "SELECT a,b,c,d FROM t" );

    bool correct_type = is_same_v<decltype( vs ), vector<S>>;
    TRUE_( correct_type );

    EQUALS( vs.size(), 2 );

    EQUALS( vs[0].a, 4       );
    EQUALS( vs[0].b, 3       );
    EQUALS( vs[0].c, "hello" );
    EQUALS( vs[0].d, 2.3     );

    EQUALS( vs[1].a, 3       );
    EQUALS( vs[1].b, 4       );
    EQUALS( vs[1].c, "hxllo" );
    EQUALS( vs[1].d, 2.1     );
}

} // namespace testing
