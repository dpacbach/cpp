/****************************************************************
* Unit tests
****************************************************************/
#include "common-test.hpp"

#include "string-util.hpp"

using namespace std;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( from_string )
{
    THROWS( util::stoi( "" ) );

    EQUALS( util::stoi( "0"      ),  0   );
    EQUALS( util::stoi( "1"      ),  1   );
    EQUALS( util::stoi( "222"    ),  222 );
    EQUALS( util::stoi( "0",  16 ),  0   );
    EQUALS( util::stoi( "10", 16 ),  16  );
    EQUALS( util::stoi( "-10"    ), -10  );
    EQUALS( util::stoi( "-0"     ),  0   );
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

#ifndef CLANG_LIBSTDCPP_NO_VARIANT
    variant<int, string> var = 5;
    EQUALS( util::to_string( var ), "5" );
    var = "hello";
    EQUALS( util::to_string( var ), "\"hello\"" );
#endif

    vector<tuple<int, string>> v3{ {5,"a"}, {6,"b"} };
    EQUALS( util::to_string( v3 ), "[(5,\"a\"),(6,\"b\")]" );

    fs::path p = "A/B/C";
    EQUALS( util::to_string( p ), "\"A/B/C\"" );

    auto now = chrono::system_clock::now();
    auto now_str = util::to_string( now );
    EQUALS( now_str.size(), 29 );
    auto now_zoned = ZonedTimePoint( now, util::tz_utc() );
    auto now_zoned_str = util::to_string( now_zoned );
    EQUALS( now_zoned_str.size(), 34 );
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

} // namespace testing
