/****************************************************************
* SVN Unit tests
****************************************************************/
#include "common-test.hpp"

#include "io.hpp"
#include "status-util.hpp"
#include "types.hpp"

using namespace std;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( status )
{
    auto f = data_common / "status.xml";

    auto v = svn::parse_status_xml( util::read_file_str( f ) );

    EQUALS( v.size(), 12 );

    EQUALS( v[0].path, "a_path/and_a_file" );
    EQUALS( v[0].change, svn::Status::ChangeType::conflicted );
    EQUALS( v[0].tree_conflict, 0 );

    EQUALS( v[1].path, "a_path/and_a_file.mine" );
    EQUALS( v[1].change, svn::Status::ChangeType::unversioned );
    EQUALS( v[1].tree_conflict, 0 );

    EQUALS( v[2].path, "a_path/and_a_file.r10" );
    EQUALS( v[2].change, svn::Status::ChangeType::unversioned );
    EQUALS( v[2].tree_conflict, 0 );

    EQUALS( v[3].path, "a_path/and_a_file.r11" );
    EQUALS( v[3].change, svn::Status::ChangeType::unversioned );
    EQUALS( v[3].tree_conflict, 0 );

    EQUALS( v[4].path, "added_file.abc" );
    EQUALS( v[4].change, svn::Status::ChangeType::added );
    EQUALS( v[4].tree_conflict, 0 );

    EQUALS( v[5].path, "another_file.py" );
    EQUALS( v[5].change, svn::Status::ChangeType::modified );
    EQUALS( v[5].tree_conflict, 0 );

    EQUALS( v[6].path, "funcs.sh" );
    EQUALS( v[6].change, svn::Status::ChangeType::deleted );
    EQUALS( v[6].tree_conflict, 0 );

    EQUALS( v[7].path, "python.py" );
    EQUALS( v[7].change, svn::Status::ChangeType::modified );
    EQUALS( v[7].tree_conflict, 0 );

    EQUALS( v[8].path, "script.sh" );
    EQUALS( v[8].change, svn::Status::ChangeType::unversioned );
    EQUALS( v[8].tree_conflict, 0 );

    EQUALS( v[9].path, "tc_d" );
    EQUALS( v[9].change, svn::Status::ChangeType::modified );
    EQUALS( v[9].tree_conflict, 1 );

    EQUALS( v[10].path, "test.py" );
    EQUALS( v[10].change, svn::Status::ChangeType::unversioned );
    EQUALS( v[10].tree_conflict, 0 );

    EQUALS( v[11].path, "text_file.txt" );
    EQUALS( v[11].change, svn::Status::ChangeType::unversioned );
    EQUALS( v[11].tree_conflict, 0 );
}

} // namespace testing
