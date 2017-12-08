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
    // Absolute paths
    EQUALS( util::lexically_normal( "/"                  ), "/"          );
    EQUALS( util::lexically_normal( "/a"                 ), "/a"         );
    EQUALS( util::lexically_normal( "/.."                ), "/"          );
    EQUALS( util::lexically_normal( "/../"               ), "/"          );
    EQUALS( util::lexically_normal( "/../../../"         ), "/"          );
    EQUALS( util::lexically_normal( "/..//../c/."        ), "/c"         );
    EQUALS( util::lexically_normal( "/.//../../."        ), "/"          );
    EQUALS( util::lexically_normal( "/a/b/c/../../c"     ), "/a/c"       );
    EQUALS( util::lexically_normal( "/a/b/c/../../../"   ), "/"          );
    EQUALS( util::lexically_normal( "/a/b/../../../../"  ), "/"          );
    EQUALS( util::lexically_normal( "/aa/bb/cc/./../x/y" ), "/aa/bb/x/y" );

    // Relative paths
    EQUALS( util::lexically_normal( ""                  ), "."         );
    EQUALS( util::lexically_normal( "a"                 ), "a"         );
    EQUALS( util::lexically_normal( ".."                ), ".."        );
    EQUALS( util::lexically_normal( "../"               ), ".."        );
    EQUALS( util::lexically_normal( "../../../"         ), "../../.."  );
    EQUALS( util::lexically_normal( "..//../c/."        ), "../../c"   );
    EQUALS( util::lexically_normal( ".//../../."        ), "../.."     );
    EQUALS( util::lexically_normal( "a/b/c/../../c"     ), "a/c"       );
    EQUALS( util::lexically_normal( "a/b/c/../../../"   ), "."         );
    EQUALS( util::lexically_normal( "a/b/../../../../"  ), "../.."     );
    EQUALS( util::lexically_normal( "aa/bb/cc/./../x/y" ), "aa/bb/x/y" );
}

void run_tests() {

    auto tests = { test_always_succeeds,
                   test_lexically_normal };
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
