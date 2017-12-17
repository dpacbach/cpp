/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "project.hpp"
#include "solution.hpp"
#include "stopwatch.hpp"
#include "string-util.hpp"
#include "xml-utils.hpp"

#include <algorithm>
#include <string>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

namespace pr = project;

//auto project_file  = "../sln-demo/samples/pugixml_vs2013.vcxproj";
//auto solution_file = "../sln-demo/libmemcached-win/win32/libmemcached.sln";
auto solution_file = "../sln-demo/C++/Calculator.sln";

int main() try {

    cout << endl;

    //fs::path rel = fs::current_path();
    fs::path rel = fs::canonical( fs::absolute( ".." ) );

    auto s = TIMEIT( "parse solution",
        pr::Solution::read( solution_file, "Debug|Win32", rel )
    );

    cout << s << endl;

    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
}
