/****************************************************************
* Unit tests for filesystem-related code
****************************************************************/
#include "precomp.hpp"
#include "common-test.hpp"

#include "fs.hpp"
#include "line-endings.hpp"
#include "logger.hpp"
#include "io.hpp"
#include "string-util.hpp"
#include "util.hpp"

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

TEST( slashes )
{
    EQUALS( util::fwd_slashes( "" ), "" );
    EQUALS( util::back_slashes( "" ), "" );

    EQUALS( util::fwd_slashes( "/"), "/" );
    EQUALS( util::back_slashes( "\\" ), "\\" );

    EQUALS( util::fwd_slashes( "////" ), "////" );
    EQUALS( util::back_slashes( "\\\\\\\\" ), "\\\\\\\\" );

    EQUALS( util::back_slashes( "////" ), "\\\\\\\\" );
    EQUALS( util::fwd_slashes( "\\\\\\\\" ), "////" );

    EQUALS( util::back_slashes( "1/a/b/c/d" ), "1\\a\\b\\c\\d" );
    EQUALS( util::fwd_slashes( "1\\2\\3\\4\\5" ), "1/2/3/4/5" );
}

TEST( dos_to_from_unix )
{
    using namespace std::chrono_literals;

    // Ideally, this must be  a  time  delta  that  is small, but
    // large enough to appear in file time stamps. And  on  Linux
    // that's what it is.  But  on  Windows,  it appears that the
    // time points we get from file time stamps don't contain any
    // sub-second  information,  so we need to make it at least a
    // second on that platform.
#ifdef OS_LINUX
    auto const delta = 10ms;
#else
    auto const delta = 1001ms;
#endif

    // First test some edge cases with dos2unix.
    vector<char> v1{}, v2{ 0x0A }, v3{ 0x0D }, v4{ 0x0D, 0x0A };
    vector<char> v5{ 0x0A, 0x0D };
    util::dos2unix( v1 ); util::dos2unix( v2 );
    util::dos2unix( v3 ); util::dos2unix( v4 );
    util::dos2unix( v5 );
    EQUALS( v1, (vector<char>{ }) );
    EQUALS( v2, (vector<char>{ 0x0A }) );
    EQUALS( v3, (vector<char>{ }) );
    EQUALS( v4, (vector<char>{ 0x0A }) );
    EQUALS( v5, (vector<char>{ 0x0A }) );

    // Now test some edge cases with unix2dos.
    vector<char> v6{}, v7{ 0x0A }, v8{ 0x0D }, v9{ 0x0D, 0x0A };
    vector<char> v10{ 0x0A, 0x0D };
    util::unix2dos( v6 ); util::unix2dos( v7 );
    util::unix2dos( v8 ); util::unix2dos( v9 );
    util::unix2dos( v10 );
    EQUALS( v6,  (vector<char>{ }) );
    EQUALS( v7,  (vector<char>{ 0x0D, 0x0A }) );
    EQUALS( v8,  (vector<char>{ 0x0D }) );
    EQUALS( v9,  (vector<char>{ 0x0D, 0x0A }) );
    EQUALS( v10, (vector<char>{ 0x0D, 0x0A, 0x0D }) );

    // Test dos2unix( vector )
    auto unix = util::read_file( data_common / "lines-unix.txt" );
    auto win  = util::read_file( data_common / "lines-win.txt"  );

    // Sanity check to make  sure  the  files have different line
    // endings  to  begin  with (e.g., that the repository hasn't
    // auto converted them for us which we don't want).
    TRUE_( win.size() > unix.size() );

    auto unix_to_unix = unix; util::dos2unix( unix_to_unix );
    auto win_to_unix  = unix; util::dos2unix( win_to_unix  );

    EQUALS( unix_to_unix, unix );
    EQUALS( win_to_unix,  unix );

    // Test dos2unix( string )
    string win_str( &win[0], win.size() );
    auto win_str_tmp = win_str;
    EQUALS( win_str.size(), 64 );
    util::dos2unix( win_str );
    EQUALS( win_str.size(), 53 );
    util::unix2dos( win_str );
    EQUALS( win_str.size(), 64 );
    EQUALS( win_str, win_str_tmp );

    // Test dos2unix( path )
    EQUALS( unix.size(), 53 );
    fs::path win_inp  = "lines-win.txt";
    fs::path unix_inp = "lines-unix.txt";
    auto win_tmp  = fs::temp_directory_path()/win_inp;
    auto unix_tmp = fs::temp_directory_path()/unix_inp;
    util::remove_if_exists( win_tmp );
    util::remove_if_exists( unix_tmp );
    util::copy_file( data_common/win_inp,  win_tmp  );
    util::copy_file( data_common/unix_inp, unix_tmp );

    auto time_0 = util::timestamp( win_tmp );
    util::timestamp( unix_tmp, time_0 );

    TRUE_( util::timestamp( win_tmp  ) ==
           util::timestamp( unix_tmp ) );

    EQUALS( fs::file_size( win_tmp  ), 64 );
    EQUALS( fs::file_size( unix_tmp ), 53 );

    this_thread::sleep_for( delta );
    EQUALS( util::dos2unix( win_tmp,  true ), true  );
    EQUALS( util::dos2unix( unix_tmp, true ), false );

    EQUALS( fs::file_size( win_tmp  ), 53 );
    EQUALS( fs::file_size( unix_tmp ), 53 );

    TRUE_( util::timestamp( unix_tmp ) == time_0 );
    TRUE_( util::timestamp( win_tmp )  == time_0 );

    auto time_1 = util::timestamp( win_tmp );

    this_thread::sleep_for( delta );
    EQUALS( util::dos2unix( win_tmp  ), false );
    EQUALS( util::dos2unix( unix_tmp ), false );

    EQUALS( fs::file_size( win_tmp  ), 53 );
    EQUALS( fs::file_size( unix_tmp ), 53 );

    TRUE_( util::timestamp( win_tmp )  == time_1 );
    TRUE_( util::timestamp( unix_tmp ) == time_1 );

    auto time_2 = util::timestamp( unix_tmp );

    this_thread::sleep_for( delta );
    EQUALS( util::unix2dos( win_tmp  ), true );
    EQUALS( util::unix2dos( unix_tmp ), true );

    EQUALS( fs::file_size( win_tmp  ), 64 );
    EQUALS( fs::file_size( unix_tmp ), 64 );

    TRUE_( util::timestamp( win_tmp )  > time_2 );
    TRUE_( util::timestamp( unix_tmp ) > time_2 );
}

TEST( touch )
{
    auto p = fs::temp_directory_path();
    util::log << "temp folder: " << p << "\n";
    auto base_time = util::timestamp( p );

    fs::path t1 = p / "AbCdEfGhIjK";
    fs::path t2 = p / "bCdEfGhIjKl/MnOp";

    util::remove_if_exists( t1 );

    ASSERT_( !fs::exists( t1 ) );

    util::touch( t1 );
    ASSERT( fs::exists( t1 ), t1 << " does not exist" );

    // Check  that  the time stamp on the file we just touched is
    // later  than  the original timestamp on temp folder (before
    // we created the file in it).
    bool gt = util::timestamp( t1 ) > base_time;
    EQUALS( gt, true );

    // Check that an attempt to touch a file  in  a  non-existent
    // folder will throw.
    THROWS( util::touch( t2 ) );
}

TEST( read_write_file )
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

    // Now write to a file and read it back in to verify.
    vector<char> v3{ 3, 4, 5, 6, 7 };

    auto p = fs::temp_directory_path() / "34567";
    util::remove_if_exists( p );

    util::write_file( p, v3 );
    TRUE_( fs::exists( p ) );
    EQUALS( fs::file_size( p ), v3.size() );

    auto v4 = util::read_file( p );
    EQUALS( v3, v4 );

    auto copy = fs::temp_directory_path()/"3-lines.txt";
    util::copy_file( data_common/"3-lines.txt", copy );
    EQUALS( fs::file_size( copy ), 21 );
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
