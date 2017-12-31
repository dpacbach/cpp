/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "logger.hpp"
#include "stopwatch.hpp"

#include <string>
#include <iostream>
#include <vector>

using namespace std;
using util::operator<<;

int main() try {

    util::Logger::enabled = true;

    util::log << "finished.\n";

    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
}
