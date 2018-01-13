/****************************************************************
* Unit tests for filesystem-related code
****************************************************************/
#include "precomp.hpp"
#include "common-test.hpp"

using namespace std;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

// Utility macro used to wrap string literals containing absolute
// paths; on windows it will attach a root name  to  them  (since
// our path manipulation functions  do  not  support paths with a
// root directory but not a root  name) and does nothing on Linux.
#ifdef _WIN32
#    define A( s ) "C:" s
#else
#    define A( s )      s
#endif

namespace testing {

TEST( touch )
{
    auto p = fs::temp_directory_path();
    util::log << "temp folder: " << p << "\n";
    auto base_time = fs::last_write_time( p );

    fs::path t1 = p / "AbCdEfGhIjK";
    fs::path t2 = p / "bCdEfGhIjKl/MnOp";

    if( fs::exists( t1 ) )
        fs::remove( t1 );

    ASSERT_( !fs::exists( t1 ) );

    util::touch( t1 );
    ASSERT( fs::exists( t1 ), t1 << " does not exist" );

    // Check  that  the time stamp on the file we just touched is
    // later  than  the original timestamp on temp folder (before
    // we created the file in it).
    bool gt = fs::last_write_time( t1 ) > base_time;
    EQUALS( gt, true );

    // Check that an attempt to touch a file  in  a  non-existent
    // folder will throw.
    THROWS( util::touch( t2 ) );
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
    b = util::path_equals( "A//B///C//", "A/B/C", sens ); EQUALS( b, true  );
    b = util::path_equals( "a/b/c", "A/B/C",      sens ); EQUALS( b, false );
    b = util::path_equals( "A", "a",              sens ); EQUALS( b, false );

    b = util::path_equals(    "A/B/C",  A( "/A/B/C" ), sens ); EQUALS( b, false );
    b = util::path_equals( A( "/abc" ), A( "/abc"   ), sens ); EQUALS( b, true  );
    b = util::path_equals( A( "/ABC" ), A( "/abc"   ), sens ); EQUALS( b, false );

    sens = util::CaseSensitive::NO;

    b = util::path_equals( "", "",                sens ); EQUALS( b, true  );
    b = util::path_equals( "A", "",               sens ); EQUALS( b, false );
    b = util::path_equals( "", "A",               sens ); EQUALS( b, false );
    b = util::path_equals( "A/B", "A",            sens ); EQUALS( b, false );
    b = util::path_equals( "A", "A/B",            sens ); EQUALS( b, false );
    b = util::path_equals( "A/B", "A/B",          sens ); EQUALS( b, true  );
    b = util::path_equals( "A//B///C//", "A/B/C", sens ); EQUALS( b, true  );
    b = util::path_equals( "a/b/c", "A/B/C",      sens ); EQUALS( b, true  );
    b = util::path_equals( "A", "a",              sens ); EQUALS( b, true  );

    b = util::path_equals(    "A/B/C",  A( "/A/B/C" ), sens ); EQUALS( b, false );
    b = util::path_equals( A( "/abc" ), A( "/abc"   ), sens ); EQUALS( b, true  );
    b = util::path_equals( A( "/ABC" ), A( "/abc"   ), sens ); EQUALS( b, true  );
}

TEST( lexically_normal )
{
    auto f = util::lexically_normal;

    // Absolute paths
    EQUALS( f( A( "/"                  )  ), A( "/"          )  );
    EQUALS( f( A( "/a"                 )  ), A( "/a"         )  );
    EQUALS( f( A( "/.."                )  ), A( "/"          )  );
    EQUALS( f( A( "/../"               )  ), A( "/"          )  );
    EQUALS( f( A( "/../../../"         )  ), A( "/"          )  );
    EQUALS( f( A( "/..//../c/."        )  ), A( "/c"         )  );
    EQUALS( f( A( "/.//../../."        )  ), A( "/"          )  );
    EQUALS( f( A( "/a/b/c/../../c"     )  ), A( "/a/c"       )  );
    EQUALS( f( A( "/a/b/c/../../../"   )  ), A( "/"          )  );
    EQUALS( f( A( "/a/b/../../../../"  )  ), A( "/"          )  );
    EQUALS( f( A( "/aa/bb/cc/./../x/y" )  ), A( "/aa/bb/x/y" )  );

#ifdef _WIN32
    THROWS( f( "C:abc" ) );
    THROWS( f( "/abc"  ) );
#endif

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

    EQUALS( f( ".", ""  ), "." );
    EQUALS( f( "",  "." ), "." );
    EQUALS( f( ".", "." ), "." );

    EQUALS( f( "..", ""   ), ".." );
    EQUALS( f( ".",  ".." ), ""   );
    EQUALS( f( "..", "."  ), ".." );
    EQUALS( f( "..", ".." ), "."  );

    EQUALS( f( "a", ""  ), "a"  );
    EQUALS( f( "",  "a" ), ".." );
    EQUALS( f( "a", "a" ), "."  );

    EQUALS( f( "a", "b"   ), "../a"    );
    EQUALS( f( "a", "b/b" ), "../../a" );
    EQUALS( f( "a", "b/a" ), "../../a" );
    EQUALS( f( "a", "a/b" ), ".."      );

    EQUALS( f( "..",       "a" ), "../.."       );
    EQUALS( f( "../..",    "a" ), "../../.."    );
    EQUALS( f( "../../..", "a" ), "../../../.." );

    EQUALS( f( "..",       "a/b/c" ), "../../../.."       );
    EQUALS( f( "../..",    "a/b/c" ), "../../../../.."    );
    EQUALS( f( "../../..", "a/b/c" ), "../../../../../.." );

    EQUALS( f( ".", "../../.." ), "" );

    EQUALS( f( ".",  "../a" ), ""   );
    EQUALS( f( ".",  "a/.." ), "."  );
    EQUALS( f( "..", "../a" ), ".." );
    EQUALS( f( "..", "a/.." ), ".." );

    EQUALS( f( "..",       "a/b/c/.."          ), "../../.." );
    EQUALS( f( "../..",    "a/b/c/../.."       ), "../../.." );
    EQUALS( f( "../../..", "../../../a/b/c"    ), "../../.." );
    EQUALS( f( "../../..", "../../../../a/b/c" ), ""         );

    EQUALS( f( "..",       "a/b/../c"       ), "../../.." );
    EQUALS( f( "../..",    "a/b/../../c"    ), "../../.." );
    EQUALS( f( "../../..", "a/b/c/../../.." ), "../../.." );
    EQUALS( f( "../../..", "a/../b/../c/.." ), "../../.." );

    EQUALS( f( "a/b/c/d/e", "a/b/c/d/e" ), "."     );
    EQUALS( f( "a/b/c/d/e", "a/b/c"     ), "d/e"   );
    EQUALS( f( "a/b/c",     "a/b/c/d/e" ), "../.." );

    EQUALS( f( "a/b/x/y/z", "a/b/c/d/e" ), "../../../x/y/z"           );
    EQUALS( f( "u/v/x/y/z", "a/b/c/d/e" ), "../../../../../u/v/x/y/z" );

#ifdef _WIN32
    // Invalid absolute paths
    THROWS( f( "/",  "a"  ) );
    THROWS( f( "a",  "/"  ) );
    THROWS( f( "C:", "a"  ) );
    THROWS( f( "a",  "C:" ) );
#endif

    // Absolute paths.
    EQUALS( f( A( "/" ), A( "/" ) ), "." );

    EQUALS( f( A( "/" ), "." ), "" );
    EQUALS( f( ".", A( "/" ) ), "" );

    EQUALS( f( A( "/.." ), A( "/"   ) ), "." );
    EQUALS( f( A( "/"   ), A( "/.." ) ), "." );
    EQUALS( f( A( "/.." ), A( "/.." ) ), "." );

    EQUALS( f( A( "/a" ), A( "/"  ) ), "a"  );
    EQUALS( f( A( "/"  ), A( "/a" ) ), ".." );
    EQUALS( f( A( "/a" ), A( "/a" ) ), "."  );

    EQUALS( f( A( "/a" ), A( "/b"   ) ), "../a"    );
    EQUALS( f( A( "/a" ), A( "/b/b" ) ), "../../a" );
    EQUALS( f( A( "/a" ), A( "/b/a" ) ), "../../a" );
    EQUALS( f( A( "/a" ), A( "/a/b" ) ), ".."      );

    EQUALS( f( A( "/.."       ), A( "/a" ) ), ".." );
    EQUALS( f( A( "/../.."    ), A( "/a" ) ), ".." );
    EQUALS( f( A( "/../../.." ), A( "/a" ) ), ".." );

    EQUALS( f( A( "/.."       ), A( "/a/b/c" ) ), "../../.." );
    EQUALS( f( A( "/../.."    ), A( "/a/b/c" ) ), "../../.." );
    EQUALS( f( A( "/../../.." ), A( "/a/b/c" ) ), "../../.." );

    EQUALS( f( A( "/" ), A( "/../a" ) ), ".." );

    EQUALS( f( A( "/"   ), A( "/a/.." ) ), "."  );
    EQUALS( f( A( "/.." ), A( "/../a" ) ), ".." );
    EQUALS( f( A( "/.." ), A( "/a/.." ) ), "."  );

    EQUALS( f( A( "/.."       ), A( "/a/b/c/.."       )  ), "../.." );
    EQUALS( f( A( "/../.."    ), A( "/a/b/c/../.."    )  ), ".." );
    EQUALS( f( A( "/../../.." ), A( "/../../../a/b/c" )  ), "../../.." );

    EQUALS( f( A( "/.."       ), A( "/a/b/../c"       )   ), "../.." );
    EQUALS( f( A( "/../.."    ), A( "/a/b/../../c"    )   ), ".."    );
    EQUALS( f( A( "/../../.." ), A( "/a/b/c/../../.." )   ), "."     );
    EQUALS( f( A( "/../../.." ), A( "/a/../b/../c/.." )   ), "."     );

    EQUALS( f( A( "/a/b/c/d/e" ), A( "/a/b/c/d/e" )   ), "."     );
    EQUALS( f( A( "/a/b/c/d/e" ), A( "/a/b/c"     )   ), "d/e"   );
    EQUALS( f( A( "/a/b/c"     ), A( "/a/b/c/d/e" )   ), "../.." );

    EQUALS( f( A( "/a/b/x/y/z" ), A( "/a/b/c/d/e" )   ), "../../../x/y/z"           );
    EQUALS( f( A( "/u/v/x/y/z" ), A( "/a/b/c/d/e" )   ), "../../../../../u/v/x/y/z" );

    EQUALS( f( A( "/a/b/c/d/e" ), A( "/a/./."      )   ), "b/c/d/e" );
    EQUALS( f( A( "/a/b/c"     ), A( "/a/./c/./.." )   ), "b/c"     );
}

} // namespace testing
