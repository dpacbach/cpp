/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "project.hpp"
#include "solution.hpp"
#include "stopwatch.hpp"
#include "string-util.hpp"
#include "xml-utils.hpp"
#include "preprocessor.hpp"
#include "opt-util.hpp"

#include <algorithm>
#include <string>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

namespace pr = project;

//auto project_file  = "../sln-demo/samples/pugixml_vs2013.vcxproj";
auto solution_file = "../sln-demo/libmemcached-win/win32/libmemcached.sln";
//auto solution_file = "../sln-demo/C++/Calculator.sln";

int main() try {

    cout << endl;

    fs::path base = fs::canonical( fs::absolute( ".." ) );

    int jobs = 1;

    PathVec source_folders{
        base
    };

    auto global = TIMEIT( "build global map",
        pr::build_sources( source_folders, base );
    );

    pr::preprocess_solution( global,
                             base,
                             solution_file,
                             "Debug|Win32",
                             jobs );

    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
}
