/****************************************************************
* Unit tests for network functionality
****************************************************************/
#include "precomp.hpp"
#include "common-test.hpp"

using namespace std;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( urlencode )
{
    auto s_inp  = util::read_file_str( data_common/"encode-inp.txt"  );
    auto s_base = util::read_file_str( data_common/"encode-base.txt" );

    auto s_enc = net::url_encode( s_inp );

    EQUALS( s_enc.size(), s_base.size() );
    EQUALS( s_enc, s_base );
}

} // namespace testing
