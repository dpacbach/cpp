/****************************************************************
* Unit tests
****************************************************************/
#include "fs.hpp"
#include "test.hpp"

#include <iostream>

using namespace std;

TEST( always_succeeds ) { }

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
