/****************************************************************
* Unit tests
****************************************************************/
#include "precomp.hpp"
#include "common-test.hpp"
#include "main.hpp"

using namespace std;

namespace pr = project;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( opt_util )
{
    vector<optional<int>> v{
        { 5 }, nullopt, { 7 }, { 9 }, nullopt, { 0 }, { 1 } };

    auto res = util::cat_opts( v );
    EQUALS( res, (vector<int>{ 5, 7, 9, 0, 1 }) );
}

TEST( algo )
{
    auto find_n = []( int n, int test ) { return test < n; };

    vector<int> v1{};

    // Test with empty vector
    auto r1 = util::lower_bound( v1, LC( find_n( 5, _ ) ) );
    auto r2 = util::lower_bound( v1, LC( find_n( 0, _ ) ) );
    TRUE_( r1 == end( v1 ) );
    TRUE_( r2 == end( v1 ) );

    // Test with vector with one element.
    vector v2{ 4 };

    auto r3 = util::lower_bound( v2, LC( find_n( 5, _ ) ) );
    auto r4 = util::lower_bound( v2, LC( find_n( 3, _ ) ) );
    auto r5 = util::lower_bound( v2, LC( find_n( 4, _ ) ) );
    TRUE_( r3 == end( v2 ) );
    TRUE_( r4 != end( v2 ) ); EQUALS( *r4, 4 );
    TRUE_( r5 != end( v2 ) ); EQUALS( *r5, 4 );

    // Test with vector with two elements.
    vector v3{ 4, 10 };
    auto r6  = util::lower_bound( v3, LC( find_n( 0,  _ ) ) );
    auto r7  = util::lower_bound( v3, LC( find_n( 3,  _ ) ) );
    auto r8  = util::lower_bound( v3, LC( find_n( 4,  _ ) ) );
    auto r9  = util::lower_bound( v3, LC( find_n( 5,  _ ) ) );
    auto r10 = util::lower_bound( v3, LC( find_n( 9,  _ ) ) );
    auto r11 = util::lower_bound( v3, LC( find_n( 10, _ ) ) );
    auto r12 = util::lower_bound( v3, LC( find_n( 11, _ ) ) );
    auto r13 = util::lower_bound( v3, LC( find_n( 12, _ ) ) );
    TRUE_( r6  != end( v3 ) ); EQUALS( *r6,  4  );
    TRUE_( r7  != end( v3 ) ); EQUALS( *r7,  4  );
    TRUE_( r8  != end( v3 ) ); EQUALS( *r8,  4  );
    TRUE_( r9  != end( v3 ) ); EQUALS( *r9,  10 );
    TRUE_( r10 != end( v3 ) ); EQUALS( *r10, 10 );
    TRUE_( r11 != end( v3 ) ); EQUALS( *r11, 10 );
    TRUE_( r12 == end( v3 ) );
    TRUE_( r13 == end( v3 ) );

    // Test with vector with many elements.
    vector v4{ 0, 4, 7, 9, 55, 102, 103, 104, 200 };

    auto r14 = util::lower_bound( v4, LC( find_n( -1,    _ ) ) );
    auto r15 = util::lower_bound( v4, LC( find_n(  0,    _ ) ) );
    auto r16 = util::lower_bound( v4, LC( find_n(  4,    _ ) ) );
    auto r17 = util::lower_bound( v4, LC( find_n(  5,    _ ) ) );
    auto r18 = util::lower_bound( v4, LC( find_n(  101,  _ ) ) );
    auto r19 = util::lower_bound( v4, LC( find_n(  102,  _ ) ) );
    auto r20 = util::lower_bound( v4, LC( find_n(  103,  _ ) ) );
    auto r21 = util::lower_bound( v4, LC( find_n(  104,  _ ) ) );
    auto r22 = util::lower_bound( v4, LC( find_n(  105,  _ ) ) );
    auto r23 = util::lower_bound( v4, LC( find_n(  106,  _ ) ) );
    auto r24 = util::lower_bound( v4, LC( find_n(  200,  _ ) ) );
    auto r25 = util::lower_bound( v4, LC( find_n(  220,  _ ) ) );

    TRUE_( r14 != end( v4 ) ); EQUALS( *r14, 0   );
    TRUE_( r15 != end( v4 ) ); EQUALS( *r15, 0   );
    TRUE_( r16 != end( v4 ) ); EQUALS( *r16, 4   );
    TRUE_( r17 != end( v4 ) ); EQUALS( *r17, 7   );
    TRUE_( r18 != end( v4 ) ); EQUALS( *r18, 102 );
    TRUE_( r19 != end( v4 ) ); EQUALS( *r19, 102 );
    TRUE_( r20 != end( v4 ) ); EQUALS( *r20, 103 );
    TRUE_( r21 != end( v4 ) ); EQUALS( *r21, 104 );
    TRUE_( r22 != end( v4 ) ); EQUALS( *r22, 200 );
    TRUE_( r23 != end( v4 ) ); EQUALS( *r23, 200 );
    TRUE_( r24 != end( v4 ) ); EQUALS( *r24, 200 );
    TRUE_( r25 == end( v4 ) );
}

TEST( md5 )
{
    vector<char> v;
    EQUALS( crypto::md5( v ).size(), 32 );
    EQUALS(
        crypto::md5(
            v
        ), "d41d8cd98f00b204e9800998ecf8427e"
    );

    vector<char> v2{ 'a', 'b', 'c' };
    EQUALS( crypto::md5( v2 ).size(), 32 );
    EQUALS(
        crypto::md5(
            v2
        ), "900150983cd24fb0d6963f7d28e17f72"
    );

    EQUALS(
        crypto::md5(
           ""
        ), "d41d8cd98f00b204e9800998ecf8427e"
    );

    EQUALS(
        crypto::md5(
           "a"
        ), "0cc175b9c0f1b6a831c399e269772661"
    );

    EQUALS(
        crypto::md5(
           "abc"
        ), "900150983cd24fb0d6963f7d28e17f72"
    );

    EQUALS(
        crypto::md5(
           "message digest"
        ), "f96b697d7cb7938d525a2f31aaf161d0"
    );

    EQUALS(
        crypto::md5(
           "abcdefghijklmnopqrstuvwxyz"
        ), "c3fcd3d76192e4007dfb496cca67e13b"
    );

    EQUALS(
        crypto::md5(
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
           "fghijklmnopqrstuvwxyz0123456789"
        ), "d174ab98d277d9f5a5611c2c9f419d9f"
    );

    EQUALS(
        crypto::md5(
           "1234567890123456789012345678901234567890"
           "1234567890123456789012345678901234567890"
        ), "57edf4a22be3c955ac49da2e2107b67a"
    );

    EQUALS(
        crypto::md5(
           "This string is precisely 56 characters "
           "long for a reason"
        ), "93d268e9bef6608ff1a6a96adbeee106"
    );

    EQUALS(
        crypto::md5(
           "This string is exactly 64 characters long "
           "for a very good reason"
        ), "655c37c2c8451a60306d09f2971e49ff"
    );

    EQUALS(
        crypto::md5(
           "This string is also a specific length.  "
           "It is exactly 128 characters long for a "
           "very good reason as well. We are testing "
           "bounds."
        ), "2ac62baa5be7fa36587c55691c026b35"
    );

    EQUALS(
        crypto::md5(
           "aaaaaaaaaa"
        ), "e09c80c42fda55f9d992e59ca6b3307d"
    );

    EQUALS(
        crypto::md5(
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        ), "014842d480b571495a4a0363793f7367"
    );

    EQUALS(
        crypto::md5(
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaa"
        ), "b64e4f62e3e14317e3a90f9ff2cde576"
    );

    // Test computing the checksum of  random  binary  data  read
    // from a file.
    vector<char> bin = util::read_file( data_common / "random.bin" );
    EQUALS( crypto::md5( bin ), "e4cf202b4e919fc8c68ca2753fc8d737" );
}

TEST( chunking )
{
    using PType = PairVec<size_t, size_t>;

    THROWS( util::chunks( 1, 0 ) );

    EQUALS( util::chunks( 0, 0 ),
            (PType{}) );

    EQUALS( util::chunks( 0, 1 ),
            (PType{}) );
    EQUALS( util::chunks( 0, 3 ),
            (PType{}) );

    EQUALS( util::chunks( 1, 1 ),
            (PType{ {0,1} }) );
    EQUALS( util::chunks( 2, 1 ),
            (PType{ {0,1},{1,2} }) );
    EQUALS( util::chunks( 3, 1 ),
            (PType{ {0,1},{1,2},{2,3} }) );

    EQUALS( util::chunks( 10, 1 ),
            (PType{ {0,1},{1,2},{2,3},{3,4},{4,5},
                    {5,6},{6,7},{7,8},{8,9},{9,10} }) );
    EQUALS( util::chunks( 10, 2 ),
            (PType{ {0,2},{2,4},{4,6},{6,8},{8,10} }) );
    EQUALS( util::chunks( 10, 3 ),
            (PType{ {0,3},{3,6},{6,9},{9,10} }) );
    EQUALS( util::chunks( 10, 4 ),
            (PType{ {0,4},{4,8},{8,10} }) );
    EQUALS( util::chunks( 10, 5 ),
            (PType{ {0,5},{5,10} }) );
    EQUALS( util::chunks( 10, 6 ),
            (PType{ {0,6},{6,10} }) );
    EQUALS( util::chunks( 10, 7 ),
            (PType{ {0,7},{7,10} }) );
    EQUALS( util::chunks( 10, 8 ),
            (PType{ {0,8},{8,10} }) );
    EQUALS( util::chunks( 10, 9 ),
            (PType{ {0,9},{9,10} }) );
    EQUALS( util::chunks( 10, 10 ),
            (PType{ {0,10} }) );
    EQUALS( util::chunks( 10, 11 ),
            (PType{ {0,10} }) );
    EQUALS( util::chunks( 10, 20 ),
            (PType{ {0,10} }) );
}

TEST( read_file )
{
    auto f = data_common / "3-lines.txt";

    auto v = util::read_file_lines( f );
    EQUALS( v, (StrVec{ "line 1", "line 2", "line 3" }) );

    auto s = util::read_file_str( f );
    EQUALS( s, "line 1\nline 2\nline 3" );

    // read file as vector of char.
    vector<char> v2 = util::read_file( data_common/"random.bin" );
    EQUALS( v2.size(), 1920 );

    // Test a random byte in the file.
    EQUALS( v2[0x6c0], char( 0x6b ) );
}

TEST( split_join )
{
    SVVec v_{ "" };
    EQUALS( util::join( v_, "," ), "" );

    SVVec v0{ "one" };
    EQUALS( util::join( v0, "," ), "one" );

    SVVec v{ "one", "two", "three" };
    EQUALS( util::join( v, "," ), "one,two,three" );
    EQUALS( util::join( v, "--" ), "one--two--three" );

    SVVec svv_{ "" };
    EQUALS( util::split( "", ',' ), svv_ );

    SVVec svv0{ "ab" };
    EQUALS( util::split( "ab", ',' ), svv0 );

    SVVec svv{ "ab", "cd", "ef" };
    EQUALS( util::split( "ab,cd,ef", ',' ), svv );

    EQUALS( util::join( util::split( "ab,cd,ef", ',' ), "," ),
            "ab,cd,ef" );
}

TEST( to_string )
{
    EQUALS( util::to_string( 5    ), "5"        );
    EQUALS( util::to_string( 5.5  ), "5.500000" );
    EQUALS( util::to_string( true ), "1"        );
    EQUALS( util::to_string( 'a'  ), "'a'"      );
    EQUALS( util::to_string( "a"  ), "\"a\""    );

    string s = "xyz";
    EQUALS( util::to_string( s ), "\"xyz\"" );

    tuple<int, string, double> tp{ 5, "david", 67.9 };
    vector<int> v1{ 3, 4, 5 };
    vector<string> v2{ "A", "B", "C" };

    EQUALS( util::to_string( tp ), "(5,\"david\",67.900000)" );
    EQUALS( util::to_string( v1 ), "[3,4,5]"                 );
    EQUALS( util::to_string( v2 ), "[\"A\",\"B\",\"C\"]"     );

    char c = 'c'; auto rw = cref( c );
    EQUALS( util::to_string( rw ), "'c'" );

    OptStr opt;
    EQUALS( util::to_string( opt ), "nullopt" );
    opt = "something";
    EQUALS( util::to_string( opt ), "\"something\"" );

    variant<int, string> var = 5;
    EQUALS( util::to_string( var ), "5" );
    var = "hello";
    EQUALS( util::to_string( var ), "\"hello\"" );

    vector<tuple<int, string>> v3{ {5,"a"}, {6,"b"} };
    EQUALS( util::to_string( v3 ), "[(5,\"a\"),(6,\"b\")]" );

    fs::path p = "A/B/C";
    EQUALS( util::to_string( p ), "\"A/B/C\"" );
}

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
    sqlite::insert_many( db,
        "INSERT INTO user (age, name, weight) VALUES (?,?,?)",
        rows
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
    sqlite::insert_many_fast( db,
        "INSERT INTO user (age, name, weight) VALUES",
        rows
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
    sqlite::insert_many( db,
        "INSERT INTO user (age) VALUES (?)",
        rows_1
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
    sqlite::insert_many_fast( db,
        "INSERT INTO user (age) VALUES",
        rows_1
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

    sqlite::insert_many_fast( db,
        "INSERT INTO user (age, name) VALUES",
        rows2
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
    sqlite::insert_many_fast( db,
        "INSERT INTO user (age, weight) VALUES", tp3, LC(
        string( "(" ) + util::to_string( get<1>( _ ) ) +
        ", " + util::to_string( 8.9 ) + ")"
    ) );

    int c; double w;
    db << "SELECT COUNT(*) FROM user WHERE age=299"        >> c;
    db << "SELECT DISTINCT weight FROM user WHERE age=299" >> w;
    EQUALS( c, 3   );
    EQUALS( w, 8.9 );
}

TEST( preprocessor )
{
#ifdef DEBUG
    // This  test  can be slow, so only run it in release testing.
    throw testing::skipped_exception();
#endif
    // NOTE:  for  this  test  we must be running with CWD of the
    // folder containing the test executable.

    auto v = util::read_file_lines(
                 data_local / "preprocessor-input.txt" );
    EQUALS( v.size(), 4 );

    fs::path cwd = fs::current_path();

    fs::path sln      = util::lexically_normal( cwd / v[0] );
    fs::path cl_read  = util::lexically_normal( cwd / v[1] );
    string   src_line = v[2]; // comma-separated list
    fs::path base     = v[3];

    PathVec srcs = util::to_paths(
        util::to_strings( util::split( src_line, ',' ) ) );

    for( auto& p : srcs )
        p = util::lexically_normal( cwd / p );

    base = util::absnormpath( base );

    if( fs::exists( cl_read ) )
        fs::remove( cl_read );

    pr::run_preprocessor(
         base,              // base folder for relative paths
         srcs,
         sln,               // path to .sln file rel to cwd
         { "Debug|Win32" }, // platforms
         0                  // jobs
    );

    TRUE( fs::exists( cl_read ), "file " << cl_read << " not "
                                 "generated." );
    auto size = fs::file_size( cl_read );
    // Just make sure something was written.
    TRUE_( size > 1000 );
}

TEST( for_each_par )
{
    vector<int> outputs{ 1, 2, 3, 4 };
    auto inc = [&outputs]( int index ){
        outputs[index]++;
    };

    util::par::for_each( vector<int>{ 0, 1, 2, 3 }, inc );
    EQUALS( outputs, (vector<int>{ 2, 3, 4, 5 }) );

    util::par::for_each( vector<int>{ 1, 3 }, inc, 1 );
    EQUALS( outputs, (vector<int>{ 2, 4, 4, 6 }) );

    util::par::for_each( vector<int>{ 1, 3 }, inc, 0 );
    EQUALS( outputs, (vector<int>{ 2, 5, 4, 7 }) );

    util::par::for_each( vector<int>{ 1, 2, 3 }, inc );
    EQUALS( outputs, (vector<int>{ 2, 6, 5, 8 }) );
}

TEST( map_par )
{
    // In this test, when creating vectors of Result's, can't use
    // initializer list directly because  for  some  reason  that
    // will require the Result variant to have  a  copy  construc-
    // tor, which  it  won't  because  the  Error  type  does not.

    auto inc = []( int x ){
        return fs::path( std::to_string( x+1 ) );
    };

    vector<int> v1;
    auto res_v1 = util::par::map( inc, v1 );
    EQUALS( res_v1, vector<fs::path>{} );

    vector<int> v2{ 3 };
    auto res_v2 = util::par::map( inc, v2 );
    EQUALS( res_v2, vector<fs::path>{ "4" } );

    vector<int> v3{ 5, 4, 3, 2, 1 };
    auto res_v3 = util::par::map( inc, v3 );
    EQUALS( res_v3, (vector<fs::path>{ "6","5","4","3","2" }) );

    // First with one job, then with  two  jobs,  then  max  jobs.
    vector<int> v4;
    vector<fs::path> goal4;
    for( int i = 0; i < 1000; ++i ) {
        v4.push_back( i );
        goal4.emplace_back( to_string( i+1 ) );
    }
    auto res_v4 = util::par::map( inc, v4, 1 );
    EQUALS( res_v4, goal4 );
    auto res_v5 = util::par::map( inc, v4, 2 );
    EQUALS( res_v5, goal4 );
    auto res_v6 = util::par::map( inc, v4, 0 );
    EQUALS( res_v6, goal4 );

    // Now test error reporting.
    vector<int> v7{ 5, 4, 3, 2, 1 };
    auto inc_err = []( int x ){
        ASSERT_( x != 3 );
        return fs::path( to_string( x+1 ) );
    };
    THROWS( util::par::map( inc_err, v7, 0 ) );
}

TEST( map_par_safe )
{
    // In this test, when creating vectors of Result's, can't use
    // initializer list directly because  for  some  reason  that
    // will require the Result variant to have  a  copy  construc-
    // tor, which  it  won't  because  the  Error  type  does not.

    auto inc = []( int x ){
        return fs::path( std::to_string( x+1 ) );
    };

    vector<int> v1;
    auto res_v1 = util::par::map_safe( inc, v1 );
    vector<util::Result<fs::path>> goal1;
    EQUALS( res_v1, goal1 );

    vector<int> v2{ 3 };
    auto res_v2 = util::par::map_safe( inc, v2 );
    vector<util::Result<fs::path>> goal2;
    goal2.emplace_back( fs::path( "4" ) );
    EQUALS( res_v2, goal2 );

    vector<int> v3{ 5, 4, 3, 2, 1 };
    auto res_v3 = util::par::map_safe( inc, v3 );
    vector<util::Result<fs::path>> goal3;
    for( auto p : { "6","5","4","3","2" } )
        goal3.emplace_back( fs::path( p ) );
    EQUALS( res_v3, goal3 );

    // First with one job, then with  two  jobs,  then  max  jobs.
    vector<int> v4;
    vector<util::Result<fs::path>> goal4;
    for( int i = 0; i < 1000; ++i ) {
        v4.push_back( i );
        goal4.emplace_back( fs::path( to_string( i+1 ) ) );
    }
    auto res_v4 = util::par::map_safe( inc, v4, 1 );
    EQUALS( res_v4, goal4 );
    auto res_v5 = util::par::map_safe( inc, v4, 2 );
    EQUALS( res_v5, goal4 );
    auto res_v6 = util::par::map_safe( inc, v4, 0 );
    EQUALS( res_v6, goal4 );

    // Now test error reporting.
    vector<int> v7{ 5, 4, 3, 2, 1 };
    auto inc_err = []( int x ){
        ASSERT_( x != 3 );
        return fs::path( to_string( x+1 ) );
    };
    auto res_v7 = util::par::map_safe( inc_err, v7, 0 );
    EQUALS( res_v7.size(), 5 );
    EQUALS( res_v7[0], util::Result<fs::path>( "6" ) );
    EQUALS( res_v7[1], util::Result<fs::path>( "5" ) );
    TRUE_( holds_alternative<util::Error>( res_v7[2] ) );
    TRUE_( util::contains(
                get<util::Error>( res_v7[2] ).msg, "error" ) );
    EQUALS( res_v7[3], util::Result<fs::path>( "3" ) );
    EQUALS( res_v7[4], util::Result<fs::path>( "2" ) );
}

TEST( resolve )
{
    // map values are not relevant to this test.
    pr::GlobalIncludeMap m{
        { "A/B/C/X.hpp",   {} },
        { "A/B/X.hpp",     {} },
        { "A/B/Y.hpp",     {} },
        { "A/B/C/Z.hpp",   {} },
        { "A/U.hpp",       {} },
        { "A/B/C/U.hpp",   {} },
        { "A/B/C/D/V.hpp", {} },
    };

    fs::path current = "A/B";

    auto _resolve = [&]( auto const& sp, auto const& rel ) {
        return pr::resolve( m, current, sp, rel );
    };

    OptCRef<fs::path> res;
    PathVec search_paths;

    search_paths = { };
    res = _resolve( search_paths, ""      ); EQUALS( res, nullopt     );
    res = _resolve( search_paths, "X"     ); EQUALS( res, nullopt     );
    res = _resolve( search_paths, "U.hpp" ); EQUALS( res, nullopt     );
    res = _resolve( search_paths, "Y.hpp" ); EQUALS( res, "A/B/Y.hpp" );

    search_paths = { "A" };
    res = _resolve( search_paths, "B/Y.hpp"   ); EQUALS( res, "A/B/Y.hpp" );
    res = _resolve( search_paths, "A/B/Y.hpp" ); EQUALS( res, nullopt     );
    res = _resolve( search_paths, "X.hpp"     ); EQUALS( res, "A/B/X.hpp" );
    res = _resolve( search_paths, "B/X.hpp"   ); EQUALS( res, "A/B/X.hpp" );
    res = _resolve( search_paths, "V.hpp"     ); EQUALS( res, nullopt     );

    search_paths = { "A/B/C" };
    res = _resolve( search_paths, "X.hpp"             ); EQUALS( res, "A/B/X.hpp"     );
    res = _resolve( search_paths, "C/X.hpp"           ); EQUALS( res, "A/B/C/X.hpp"   );
    res = _resolve( search_paths, "U.hpp"             ); EQUALS( res, "A/B/C/U.hpp"   );
    res = _resolve( search_paths, "V.hpp"             ); EQUALS( res, nullopt         );
    res = _resolve( search_paths, "A/B/C/X.hpp"       ); EQUALS( res, nullopt         );
    res = _resolve( search_paths, "C/D/Vx.hpp"        ); EQUALS( res, nullopt         );
    res = _resolve( search_paths, "C/D/V.hpp"         ); EQUALS( res, "A/B/C/D/V.hpp" );
    res = _resolve( search_paths, "Y.hpp"             ); EQUALS( res, "A/B/Y.hpp"     );
    res = _resolve( search_paths, "../C/X.hpp"        ); EQUALS( res, "A/B/C/X.hpp"   );
    res = _resolve( search_paths, "../../U.hpp"       ); EQUALS( res, "A/U.hpp"       );
    res = _resolve( search_paths, "../../../../U.hpp" ); EQUALS( res, nullopt         );

    search_paths = { "A/B/C/D", "A" };
    PathVec relatives{
        "XYZ.hpp", "V.hpp", "../A/U.hpp", "../U.hpp", "C/U.hpp", "C/X.hpp"
    };
    PathCRefVec rs = pr::resolves( m, current, search_paths, relatives );

    EQUALS( rs.size(), size_t( 5 )     );
    EQUALS( rs[0],     "A/B/C/D/V.hpp" );
    EQUALS( rs[1],     "A/U.hpp"       );
    EQUALS( rs[2],     "A/U.hpp"       );
    EQUALS( rs[3],     "A/B/C/U.hpp"   );
    EQUALS( rs[4],     "A/B/C/X.hpp"   );
}

TEST( filesystem )
{
    bool b;

    util::CaseSensitive sens = util::CaseSensitive::YES;

    b = util::path_equals( "", "",                sens ); EQUALS( b, true  );
    b = util::path_equals( "A", "",               sens ); EQUALS( b, false );
    b = util::path_equals( "", "A",               sens ); EQUALS( b, false );
    b = util::path_equals( "A/B", "A",            sens ); EQUALS( b, false );
    b = util::path_equals( "A", "A/B",            sens ); EQUALS( b, false );
    b = util::path_equals( "A/B", "A/B",          sens ); EQUALS( b, true  );
    b = util::path_equals( "A/B/C", "/A/B/C",     sens ); EQUALS( b, false );
    b = util::path_equals( "A//B///C//", "A/B/C", sens ); EQUALS( b, true  );
    b = util::path_equals( "a/b/c", "A/B/C",      sens ); EQUALS( b, false );
    b = util::path_equals( "A", "a",              sens ); EQUALS( b, false );
    b = util::path_equals( "/abc", "/abc",        sens ); EQUALS( b, true  );
    b = util::path_equals( "/ABC", "/abc",        sens ); EQUALS( b, false );

    sens = util::CaseSensitive::NO;

    b = util::path_equals( "", "",                sens ); EQUALS( b, true  );
    b = util::path_equals( "A", "",               sens ); EQUALS( b, false );
    b = util::path_equals( "", "A",               sens ); EQUALS( b, false );
    b = util::path_equals( "A/B", "A",            sens ); EQUALS( b, false );
    b = util::path_equals( "A", "A/B",            sens ); EQUALS( b, false );
    b = util::path_equals( "A/B", "A/B",          sens ); EQUALS( b, true  );
    b = util::path_equals( "A/B/C", "/A/B/C",     sens ); EQUALS( b, false );
    b = util::path_equals( "A//B///C//", "A/B/C", sens ); EQUALS( b, true  );
    b = util::path_equals( "a/b/c", "A/B/C",      sens ); EQUALS( b, true  );
    b = util::path_equals( "A", "a",              sens ); EQUALS( b, true  );
    b = util::path_equals( "/abc", "/abc",        sens ); EQUALS( b, true  );
    b = util::path_equals( "/ABC", "/abc",        sens ); EQUALS( b, true  );
}

TEST( string_util )
{
    bool b;

    /*************************************************************
    * starts_with / ends_with
    *************************************************************/
    b = util::starts_with( ""      , ""      ); EQUALS( b, true  );
    b = util::starts_with( "x"     , ""      ); EQUALS( b, true  );
    b = util::starts_with( ""      , "x"     ); EQUALS( b, false );
    b = util::starts_with( "xxx"   , ""      ); EQUALS( b, true  );
    b = util::starts_with( ""      , "xxx"   ); EQUALS( b, false );
    b = util::starts_with( "abcde" , "abcde" ); EQUALS( b, true  );
    b = util::starts_with( "abcde" , "a"     ); EQUALS( b, true  );
    b = util::starts_with( "abcde" , "ab"    ); EQUALS( b, true  );
    b = util::starts_with( "abcde" , "abcd"  ); EQUALS( b, true  );
    b = util::starts_with( "abcde" , "abfd"  ); EQUALS( b, false );
    b = util::starts_with( "abcde" , "abfdx" ); EQUALS( b, false );
    b = util::starts_with( "abcde ", "abcd"  ); EQUALS( b, true  );

    b = util::ends_with( ""      , ""      ); EQUALS( b, true  );
    b = util::ends_with( "x"     , ""      ); EQUALS( b, true  );
    b = util::ends_with( ""      , "x"     ); EQUALS( b, false );
    b = util::ends_with( "xxx"   , ""      ); EQUALS( b, true  );
    b = util::ends_with( ""      , "xxx"   ); EQUALS( b, false );
    b = util::ends_with( "abcde" , "abcde" ); EQUALS( b, true  );
    b = util::ends_with( "abcde" , "e"     ); EQUALS( b, true  );
    b = util::ends_with( "abcde" , "de"    ); EQUALS( b, true  );
    b = util::ends_with( "abcde" , "bcde"  ); EQUALS( b, true  );
    b = util::ends_with( "abcde" , "bcfe"  ); EQUALS( b, false );
    b = util::ends_with( "abcde" , "xbcfe" ); EQUALS( b, false );
    b = util::ends_with( " abcde", "bcde"  ); EQUALS( b, true  );

    /*************************************************************
    * string comparison
    *************************************************************/
    b = util::iequals<string>( ""      , ""       ); EQUALS( b, true  );
    b = util::iequals<string>( "x"     , ""       ); EQUALS( b, false );
    b = util::iequals<string>( ""      , "x"      ); EQUALS( b, false );
    b = util::iequals<string>( "x"     , "x"      ); EQUALS( b, true  );
    b = util::iequals<string>( "X"     , "x"      ); EQUALS( b, true  );
    b = util::iequals<string>( "x"     , "X"      ); EQUALS( b, true  );
    b = util::iequals<string>( "abcde" , "abcde"  ); EQUALS( b, true  );
    b = util::iequals<string>( "aBCde" , "abcde"  ); EQUALS( b, true  );
    b = util::iequals<string>( "abcde" , "abcdex" ); EQUALS( b, false );
    b = util::iequals<string>( "abcdex", "abcde"  ); EQUALS( b, false );
    b = util::iequals<string>( "abcde" , "xabcde" ); EQUALS( b, false );
    b = util::iequals<string>( "xabcde", "abcde"  ); EQUALS( b, false );
    b = util::iequals<string>( "ABCDE",  "abcde"  ); EQUALS( b, true  );
}

TEST( include_scan )
{
    vector<string> good{
        "#include <A/B/C/D.hpp>",
        "#include \"A/B/C/D.hpp\"",
        "  #  include   \"A/B/C/D.hpp\"  ",
        "#include<A/B/C/D.hpp\"",
        "#include <A/B/C/D.hpp>",
        "#include <A/B/C/D.hpp>"
        "#include <A/B/C/D.hpp> // with a comment",
        "#include <A/B/C/D.hpp> // with a comment with \"quotes\"",
        "#include <A/B/C/D.hpp> // include <commented/out>",
    };
    vector<string> bad{
        "#include A/B/C/D.hpp>",
        " include A/B/C/D.hpp>",
        "#include",
        "# A/B/C/D.hpp>",
        "#incude A/B/C/D.hpp>",
        "#include <A/B/C/D.hpp   "
        "#include <A/B/C/D.hpp   "
    };

    for( auto const& v : good ) {
        EQUALS( pr::parse_include( v ), "A/B/C/D.hpp" );
    }
    for( auto const& v : bad ) {
        EQUALS( pr::parse_include( v ), nullopt );
    }
}

TEST( directed_graph )
{
    using DG = util::DirectedGraph<fs::path>;

    unordered_map<fs::path, vector<fs::path>> m{
        { "B", { "B"      } },
        { "F", { "C", "B" } },
        { "C", { "D", "E" } },
        { "D", { "E"      } },
        { "E", { "F"      } },
        { "A", { "G"      } },
        { "G", { "C"      } },
        { "H", { "C", "D" } }
    };

    DG g = util::make_graph<fs::path>( m );

    vector<fs::path> v;

    v = g.accessible( "A" );
    sort( begin( v ), end( v ) );
    EQUALS( v, (vector<fs::path>{ "A","B","C","D","E","F","G" }) );

    v = g.accessible( "E" );
    sort( begin( v ), end( v ) );
    EQUALS( v, (vector<fs::path>{ "B", "C", "D", "E", "F" }) );

    v = g.accessible( "H" );
    sort( begin( v ), end( v ) );
    EQUALS( v, (vector<fs::path>{ "B","C","D","E","F","H" }) );

    v = g.accessible( "G" );
    sort( begin( v ), end( v ) );
    EQUALS( v, (vector<fs::path>{ "B", "C", "D", "E", "F", "G" }) );

    /*************************************************************
    * reference_wrapper test
    *************************************************************/
    pr::GlobalRefIncludeMap m2;

    fs::path p1 = "A/B/1", p2 = "A/B/2", p3 = "A/B/3", p4 = "A/B/4";

    m2[p1] = { p1, p2 };
    m2[p2] = { p1, p4 };
    m2[p3] = { p1     };
    m2[p4] = { p4     };

    using DG2 = util::DirectedGraph<PathCRef>;
    DG2 g2 = util::make_graph<PathCRef>( m2 );

    PathCRefVec v2;

    v2 = g2.accessible( p4 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (PathCRefVec{ p4 }) );

    v2 = g2.accessible( p3 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (PathCRefVec{ p1, p2, p3, p4 }) );

    v2 = g2.accessible( p2 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (PathCRefVec{ p1, p2, p4 }) );

    v2 = g2.accessible( p1 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (PathCRefVec{ p1, p2, p4 }) );

}

TEST( bimap )
{
    using BM = util::BDIndexMap<fs::path>;

    BM bm0( {} );
    EQUALS( bm0.size(), size_t( 0 ) );

    auto data = vector<fs::path>{
        "A/B/C/D/E",
        "A",
        "A",
        "A",
        "A/B/C",
        "A/B",
        "A",
        "A/B/C/D",
        "A",
        "",
        "ABBB",
        "AAAA",
    };

    BM bm( move( data ) );

    EQUALS( bm.size(), size_t( 8 ) );

    fs::path s;

    for( size_t i = 0; i < bm.size(); ++i )
        { TRUE_( bm.val_safe( i ) ); }

    s = *bm.val_safe( 0 ); EQUALS( s, ""          );
    s = *bm.val_safe( 1 ); EQUALS( s, "A"         );
    s = *bm.val_safe( 2 ); EQUALS( s, "A/B"       );
    s = *bm.val_safe( 3 ); EQUALS( s, "A/B/C"     );
    s = *bm.val_safe( 4 ); EQUALS( s, "A/B/C/D"   );
    s = *bm.val_safe( 5 ); EQUALS( s, "A/B/C/D/E" );
    s = *bm.val_safe( 6 ); EQUALS( s, "AAAA"      );
    s = *bm.val_safe( 7 ); EQUALS( s, "ABBB"      );

    TRUE_( !(bm.val_safe( 8 )) );
    TRUE_( !(bm.val_safe( 8000 )) );

    EQUALS( bm.key_safe( ""          ), 0 );
    EQUALS( bm.key_safe( "A"         ), 1 );
    EQUALS( bm.key_safe( "A/B"       ), 2 );
    EQUALS( bm.key_safe( "A/B/C"     ), 3 );
    EQUALS( bm.key_safe( "A/B/C/D"   ), 4 );
    EQUALS( bm.key_safe( "A/B/C/D/E" ), 5 );
    EQUALS( bm.key_safe( "AAAA"      ), 6 );
    EQUALS( bm.key_safe( "ABBB"      ), 7 );

    EQUALS( bm.key_safe( "XXXX" ), nullopt );
    EQUALS( bm.key_safe( "AAA"  ), nullopt );

    /******************************************************/
    // Now test BiMapFixed.

    // First empty map.
    vector<tuple<string, int>> v0{};
    util::BiMapFixed bmf0( move( v0 ) );
    EQUALS( bmf0.size(), 0 );
    TRUE_( !bmf0.val_safe( "xxx" ) );
    TRUE_( !bmf0.key_safe( 1     ) );

    // Now a large map.
    vector<tuple<string, int>> v1{
        { "abc",     9    },
        { "def",     2    },
        { "yyy",     3000 },
        { "ab",      8    },
        { "xxx",     2000 },
        { "d",       3    },
        { "hello",   7    },
        { "one",     4    },
        { "two",     6    },
        { "three",   5    },
        { "33",      33   },
        { "98",      98   },
        { "",        101  }
    };

    // This will move from the vector!
    util::BiMapFixed bmf1( move( v1 ) );

    EQUALS( bmf1.size(), 13 );
    auto r1 = bmf1.val_safe( "xxx" );
    TRUE_( r1 ); EQUALS( *r1, 2000 );
    auto r2 = bmf1.val_safe( "three" );
    TRUE_( r2 ); EQUALS( *r2, 5 );
    auto r3 = bmf1.val_safe( "aaa" );
    TRUE_( !r3 );
    auto r4 = bmf1.val_safe( "" );
    TRUE_( r4 ); EQUALS( *r4, 101 );

    auto r5 = bmf1.key_safe( 101 );
    TRUE_( r5 ); EQUALS( (*r5).get(), string( "" ) );
    auto r6 = bmf1.key_safe( 3000 );
    TRUE_( r6 ); EQUALS( (*r6).get(), string( "yyy" ) );
    auto r7 = bmf1.key_safe( 6 );
    TRUE_( r7 ); EQUALS( (*r7).get(), string( "two" ) );
    auto r8 = bmf1.key_safe( 3001 );
    TRUE_( !r8 );

    THROWS( bmf1.key(  102  ) );
    THROWS( bmf1.val( "988" ) );

    EQUALS( bmf1.key(  98   ), string( "98" ) );
    EQUALS( bmf1.key(  3    ), string( "d"  ) );
    EQUALS( bmf1.val( "98"  ), 98 );
    EQUALS( bmf1.val( "d"   ), 3  );

    // test iterator interface.
    vector<tuple<string, int>> v2;
    for( auto const& [k,v] : bmf1 )
        v2.emplace_back( k, v );

    EQUALS( v2.size(), 13 );
    EQUALS( get<0>( v2[0]  ), string( "" )    );
    EQUALS( get<1>( v2[0]  ), 101             );
    EQUALS( get<0>( v2[2]  ), string( "98" )  );
    EQUALS( get<1>( v2[2]  ), 98              );
    EQUALS( get<0>( v2[12] ), string( "yyy" ) );
    EQUALS( get<1>( v2[12] ), 3000            );
}

TEST( lexically_normal )
{
    auto f = util::lexically_normal;

    // Absolute paths
    EQUALS( f( "/"                  ), "/"          );
    EQUALS( f( "/a"                 ), "/a"         );
    EQUALS( f( "/.."                ), "/"          );
    EQUALS( f( "/../"               ), "/"          );
    EQUALS( f( "/../../../"         ), "/"          );
    EQUALS( f( "/..//../c/."        ), "/c"         );
    EQUALS( f( "/.//../../."        ), "/"          );
    EQUALS( f( "/a/b/c/../../c"     ), "/a/c"       );
    EQUALS( f( "/a/b/c/../../../"   ), "/"          );
    EQUALS( f( "/a/b/../../../../"  ), "/"          );
    EQUALS( f( "/aa/bb/cc/./../x/y" ), "/aa/bb/x/y" );

    // Relative paths
    EQUALS( f( ""                  ), "."         );
    EQUALS( f( "a"                 ), "a"         );
    EQUALS( f( ".."                ), ".."        );
    EQUALS( f( "../"               ), ".."        );
    EQUALS( f( "../../../"         ), "../../.."  );
    EQUALS( f( "..//../c/."        ), "../../c"   );
    EQUALS( f( ".//../../."        ), "../.."     );
    EQUALS( f( "a/b/c/../../c"     ), "a/c"       );
    EQUALS( f( "a/b/c/../../../"   ), "."         );
    EQUALS( f( "a/b/../../../../"  ), "../.."     );
    EQUALS( f( "aa/bb/cc/./../x/y" ), "aa/bb/x/y" );
}

TEST( lexically_relative )
{
    auto f = util::lexically_relative;

    // Relative paths.
    EQUALS( f( "", "" ), "." );

    EQUALS( f( ".", "" ), "." );
    EQUALS( f( "", "." ), "." );
    EQUALS( f( ".", "." ), "." );

    EQUALS( f( "..", "" ), ".." );
    EQUALS( f( ".", ".." ), "" );
    EQUALS( f( "..", "." ), ".." );
    EQUALS( f( "..", ".." ), "." );

    EQUALS( f( "a", "" ), "a" );
    EQUALS( f( "", "a" ), ".." );
    EQUALS( f( "a", "a" ), "." );

    EQUALS( f( "a", "b" ), "../a" );
    EQUALS( f( "a", "b/b" ), "../../a" );
    EQUALS( f( "a", "b/a" ), "../../a" );
    EQUALS( f( "a", "a/b" ), ".." );

    EQUALS( f( "..", "a" ), "../.." );
    EQUALS( f( "../..", "a" ), "../../.." );
    EQUALS( f( "../../..", "a" ), "../../../.." );

    EQUALS( f( "..", "a/b/c" ), "../../../.." );
    EQUALS( f( "../..", "a/b/c" ), "../../../../.." );
    EQUALS( f( "../../..", "a/b/c" ), "../../../../../.." );

    EQUALS( f( ".", "../../.." ), "" );

    EQUALS( f( ".", "../a" ), "" );
    EQUALS( f( ".", "a/.." ), "." );
    EQUALS( f( "..", "../a" ), ".." );
    EQUALS( f( "..", "a/.." ), ".." );

    EQUALS( f( "..", "a/b/c/.." ), "../../.." );
    EQUALS( f( "../..", "a/b/c/../.." ), "../../.." );
    EQUALS( f( "../../..", "../../../a/b/c" ), "../../.." );
    EQUALS( f( "../../..", "../../../../a/b/c" ), "" );

    EQUALS( f( "..", "a/b/../c" ), "../../.." );
    EQUALS( f( "../..", "a/b/../../c" ), "../../.." );
    EQUALS( f( "../../..", "a/b/c/../../.." ), "../../.." );
    EQUALS( f( "../../..", "a/../b/../c/.." ), "../../.." );

    EQUALS( f( "a/b/c/d/e", "a/b/c/d/e" ), "." );
    EQUALS( f( "a/b/c/d/e", "a/b/c" ), "d/e" );
    EQUALS( f( "a/b/c", "a/b/c/d/e" ), "../.." );

    EQUALS( f( "a/b/x/y/z", "a/b/c/d/e" ), "../../../x/y/z" );
    EQUALS( f( "u/v/x/y/z", "a/b/c/d/e" ), "../../../../../u/v/x/y/z" );

    // Absolute paths.
    EQUALS( f( "/", "/" ), "." );

    EQUALS( f( "/", "." ), "" );
    EQUALS( f( ".", "/" ), "" );

    EQUALS( f( "/..", "/" ), ".." );
    EQUALS( f( "/", "/.." ), "" );
    EQUALS( f( "/..", "/.." ), "." );

    EQUALS( f( "/a", "/" ), "a" );
    EQUALS( f( "/", "/a" ), ".." );
    EQUALS( f( "/a", "/a" ), "." );

    EQUALS( f( "/a", "/b" ), "../a" );
    EQUALS( f( "/a", "/b/b" ), "../../a" );
    EQUALS( f( "/a", "/b/a" ), "../../a" );
    EQUALS( f( "/a", "/a/b" ), ".." );

    // With these, keep in mind that result is not required to be
    // in  normal  form (and won't). So therefore in general a so-
    // lution is not unique, so  here  we are checking it against
    // the (valid) solution that was recorded when the tests were
    // first run successfully.
    EQUALS( f( "/..", "/a" ), "../.." );
    EQUALS( f( "/../..", "/a" ), "../../.." );
    EQUALS( f( "/../../..", "/a" ), "../../../.." );

    EQUALS( f( "/..", "/a/b/c" ), "../../../.." );
    EQUALS( f( "/../..", "/a/b/c" ), "../../../../.." );
    EQUALS( f( "/../../..", "/a/b/c" ), "../../../../../.." );

    //EQUALS( f( "/", "/../a" ), ".." ); // ??? fails
    EQUALS( f( "/", "/a/.." ), "." );
    EQUALS( f( "/..", "/../a" ), ".." );
    EQUALS( f( "/..", "/a/.." ), ".." );

    EQUALS( f( "/..", "/a/b/c/.." ), "../../.." );
    EQUALS( f( "/../..", "/a/b/c/../.." ), "../../.." );
    EQUALS( f( "/../../..", "/../../../a/b/c" ), "../../.." );

    EQUALS( f( "/..", "/a/b/../c" ), "../../.." );
    EQUALS( f( "/../..", "/a/b/../../c" ), "../../.." );
    EQUALS( f( "/../../..", "/a/b/c/../../.." ), "../../.." );
    EQUALS( f( "/../../..", "/a/../b/../c/.." ), "../../.." );

    EQUALS( f( "/a/b/c/d/e", "/a/b/c/d/e" ), "." );
    EQUALS( f( "/a/b/c/d/e", "/a/b/c" ), "d/e" );
    EQUALS( f( "/a/b/c", "/a/b/c/d/e" ), "../.." );

    EQUALS( f( "/a/b/x/y/z", "/a/b/c/d/e" ), "../../../x/y/z" );
    EQUALS( f( "/u/v/x/y/z", "/a/b/c/d/e" ), "../../../../../u/v/x/y/z" );

    EQUALS( f( "/a/b/c/d/e", "/a/./." ), "b/c/d/e" );
    EQUALS( f( "/a/b/c", "/a/./c/./.." ), "b/c" );
}

TEST( lexically_relative_fast )
{
    auto f = util::lexically_relative_fast;

    // Relative paths
    EQUALS( f( "a/b/c/d/e", "a/b/c/d/e" ), "." );

    EQUALS( f( "a/b/c/d/e", "a/b/c/d/e" ), "." );
    EQUALS( f( "a/b/c/d/e", "a/b/c" ), "d/e" );
    EQUALS( f( "a/b/c/d/e", "x/y/z" ), "../../../a/b/c/d/e" );
    EQUALS( f( "a/b/c", "a/b/c/d/e" ), "../.." );

    EQUALS( f( "a/b/x/y/z", "a/b/c/d/e" ), "../../../x/y/z" );
    EQUALS( f( "u/v/x/y/z", "a/b/c/d/e" ), "../../../../../u/v/x/y/z" );

    // Absolute paths
    EQUALS( f( "/a/b/c/d/e", "/a/b/c/d/e" ), "." );

    EQUALS( f( "/a/b/c/d/e", "/a/b/c/d/e" ), "." );
    EQUALS( f( "/a/b/c/d/e", "/a/b/c" ), "d/e" );
    EQUALS( f( "/a/b/c/d/e", "/x/y/z" ), "../../../a/b/c/d/e" );
    EQUALS( f( "/a/b/c", "/a/b/c/d/e" ), "../.." );

    EQUALS( f( "/a/b/x/y/z", "/a/b/c/d/e" ), "../../../x/y/z" );
    EQUALS( f( "/u/v/x/y/z", "/a/b/c/d/e" ), "../../../../../u/v/x/y/z" );
}

} // namespace testing

int main_( int, char** )
{
    util::Logger::enabled = false;

    testing::run_all_tests();

    return 0;
}
