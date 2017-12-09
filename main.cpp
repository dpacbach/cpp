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

//auto project_file = "../xml-utils/samples/pugixml_vs2013.vcxproj";
auto project_file = "/home/dsicilia/dev/cpp/sln-demo/C++/Calculator/Calculator.vcxproj";
//auto project_file = "/Users/dsicilia/dev/libmemcached-win/win32/libmemcached.vcxproj";
auto solution_file = "/home/dsicilia/dev/cpp/sln-demo/C++/Calculator.sln";

int main() try {

    cout << endl;

    //fs::path rel = fs::current_path();
    fs::path rel = fs::canonical( fs::absolute( ".." ) );

    cout << pr::Solution::read( solution_file, "Debug|Win32", rel ) << endl;

    // path may be relative to bin folder
    //cout << pr::Project::read( project_file, rel, "Debug|Win32" ) << endl;
    //cout << pr::Project::read( project_file, rel, "Debug|x64"   ) << endl;

    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
}
