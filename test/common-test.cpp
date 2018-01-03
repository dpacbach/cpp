/****************************************************************
* Helpers for unit tests
****************************************************************/
#include "precomp.hpp"
#include "common-test.hpp"

using namespace std;

namespace testing {

// These are updated on each  macro  call  and are printed out in
// the  event  that  an exception happens that does not come from
// one of the unit testing macros  in  order  to help the user to
// figure out which line threw the exception.
size_t      checkpoint_line = 0;
char const* checkpoint_file = "none";

string fail() { return util::c_red    + "Fail" + util::c_norm; }
string pass() { return util::c_green  + "Pass" + util::c_norm; }
string skip() { return util::c_yellow + "Skip" + util::c_norm; }

string bar() {
    return "---------------------------------------------------";
}

// Global test list: tests are automatically registered and added
// to this list at static initialization time.
vector<TestType*>& test_list() {
    static vector<TestType*> g_tests;
    return g_tests;
}

void run_all_tests() {
    for( auto f : test_list() ) f();
}

} // namespace testing
