/****************************************************************
* Unit tests
****************************************************************/
#include "bimap.hpp"
#include "fs.hpp"
#include "graph.hpp"
#include "opt-util.hpp"
#include "preprocessor.hpp"
#include "string-util.hpp"
#include "test.hpp"

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

using namespace std;

namespace pr = project;

TEST( always_succeeds ) { }

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
    using RWP  = reference_wrapper<fs::path const>;
    using VRWP = vector<RWP>;

    unordered_map<RWP, vector<RWP>> m2;

    fs::path p1 = "A/B/1", p2 = "A/B/2", p3 = "A/B/3", p4 = "A/B/4";

    m2[p1] = { p1, p2 };
    m2[p2] = { p1, p4 };
    m2[p3] = { p1     };
    m2[p4] = { p4     };

    using DG2 = util::DirectedGraph<RWP>;
    DG2 g2 = util::make_graph<RWP>( m2 );

    VRWP v2;

    v2 = g2.accessible( p4 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (VRWP{ p4 }) );

    v2 = g2.accessible( p3 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (VRWP{ p1, p2, p3, p4 }) );

    v2 = g2.accessible( p2 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (VRWP{ p1, p2, p4 }) );

    v2 = g2.accessible( p1 );
    sort( begin( v2 ), end( v2 ) );
    EQUALS( v2, (VRWP{ p1, p2, p4 }) );

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
        { TRUE( bm.val_safe( i ) ); }

    s = *bm.val_safe( 0 ); EQUALS( s, ""          );
    s = *bm.val_safe( 1 ); EQUALS( s, "A"         );
    s = *bm.val_safe( 2 ); EQUALS( s, "A/B"       );
    s = *bm.val_safe( 3 ); EQUALS( s, "A/B/C"     );
    s = *bm.val_safe( 4 ); EQUALS( s, "A/B/C/D"   );
    s = *bm.val_safe( 5 ); EQUALS( s, "A/B/C/D/E" );
    s = *bm.val_safe( 6 ); EQUALS( s, "AAAA"      );
    s = *bm.val_safe( 7 ); EQUALS( s, "ABBB"      );

    TRUE( !(bm.val_safe( 8 )) );
    TRUE( !(bm.val_safe( 8000 )) );

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

void run_tests() {

    auto tests = { test_always_succeeds,
                   test_resolve,
                   test_string_util,
                   test_filesystem,
                   test_include_scan,
                   test_directed_graph,
                   test_bimap,
                   test_lexically_normal,
                   test_lexically_relative,
                   test_lexically_relative_fast };
    for( auto const& t : tests )
        t();

    cout << endl << "All Tests Passed." << endl;
}

int main() try {

    run_tests();
    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
} catch( ... ) {
    cerr << "exception unknown." << endl;
    return 1;
}
