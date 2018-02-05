/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "fs.hpp"
#include "logger.hpp"
#include "main.hpp"
#include "string-util.hpp"
#include "stopwatch.hpp"

#include <string>
#include <iostream>
#include <vector>

using namespace std;
using util::operator<<;

int main_( int, char** )
{
    util::Logger::enabled = true;

    util::log << "finished.\n";

    return 0;
}
