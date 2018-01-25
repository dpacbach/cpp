/****************************************************************
* Unit tests
****************************************************************/
#include "common-test.hpp"

#include "add-remove.hpp"
#include "io.hpp"
#include "opt-util.hpp"
#include "preprocessor.hpp"
#include "string-util.hpp"

using namespace std;

namespace pr = project;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";
fs::path const tmp         = fs::temp_directory_path();

namespace testing {

TEST( add_remove )
{
    auto sub_dir = tmp/"proj";

    fs::create_directory( sub_dir );

    auto proj = sub_dir/"sample.vcxproj";

    util::remove_if_exists( proj );

    // Copy project file since we'll be modifying it.
    util::copy_file( data_common/"sample.vcxproj", proj );

    // Doesn't have to exist for what we're about to do.
    fs::path p1 = tmp/"xyz.cpp";

    // Project file should not refer to xyz.cpp
    TRUE_( !util::contains( util::read_file_str( proj ), "xyz" ) );

    // Trying to remove xyz.cpp  should  fail  since  it  is  not
    // present in the project file.
    THROWS( pr::rm_from( proj, p1 ) );

    pr::add_2_vcxproj( proj, p1 );

    // The project file should now refer to xyz.cpp in a specific
    // way.
    TRUE_( util::contains( util::read_file_str( proj ), "..\\xyz.cpp" ) );

    // Trying to add the same source file again should fail.
    THROWS( pr::add_2_vcxproj( proj, p1 ) );

    pr::rm_from( proj, p1 );
    // Trying  to remove the the same source file twice should al-
    // ways fail, even if  originally  there  were multiple refer-
    // ences to that file in the project file.
    THROWS( pr::rm_from( proj, p1 ) );

    // The project file  should  not  contain  any  references to
    // xyz.cpp in any form.
    TRUE_( !util::contains( util::read_file_str( proj ), "xyz" ) );
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

    util::remove_if_exists( cl_read );

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

} // namespace testing
