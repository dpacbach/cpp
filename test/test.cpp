/****************************************************************
* Unit tests
****************************************************************/
#include "common-test.hpp"

#include "algo.hpp"
#include "graph.hpp"
#include "io.hpp"
#include "md5-util.hpp"
#include "opt-util.hpp"
#include "preprocessor.hpp"
#include "string-util.hpp"

using namespace std;

namespace pr = project;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( datetime )
{
    // Parenthesis in regex's are for raw string, not capture.

    // time_t overload
    auto s1 = util::fmt_time( chrono::seconds( time( NULL ) ) );
    MATCHES( s1, R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})" );

    // chrono::system_clock::time_point overload
    auto l = chrono::system_clock::now();
    auto t = util::fmt_time( l );
    MATCHES( t, R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{9})" );

    // ZonedTimePoint overload
    auto z = ZonedTimePoint( l, util::tz_utc() );
    t = util::fmt_time( z );
    MATCHES( t,
        R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{9}[-+]\d{4})" );
    t = util::fmt_time( z, util::tz_utc() );
    MATCHES( t,
        R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{9}\+0000)" );

    auto hhmm = util::tz_hhmm();
    EQUALS( hhmm.size(), 5 );
    auto hhmm_utc = util::tz_hhmm( util::tz_utc() );
    EQUALS( hhmm_utc, "+0000" );
}

TEST( opt_util )
{
    vector<optional<int>> v{
        { 5 }, nullopt, { 7 }, { 9 }, nullopt, { 0 }, { 1 } };

    auto res = util::cat_opts( v );
    EQUALS( res, (vector<int>{ 5, 7, 9, 0, 1 }) );
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

} // namespace testing
