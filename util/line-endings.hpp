/****************************************************************
* Utilities for handling line endings
****************************************************************/
#pragma once

#include "fs.hpp"
#include "util.hpp"

#include <vector>

namespace util {

// This function will simply remove and 0x0D characters from  the
// input (mutating the argument). The new size of the vector will
// therefore always be less or equal to its original size.
template<typename Container>
void dos2unix( Container&& c ) {
    util::remove_if( c, L( _ == 0x0d ) );
}

// Open the given path and edit  it to remove all 0x0D characters.
// This  attempts to emulate the command line utility of the same
// name. As with  the  command,  the  `keep_date`  flag indicates
// whether the timestamp on the  file  should remain unchanged in
// the event that the  file  contains  no  0x0D characters. By de-
// fault, the timestamp will always be touched.
void dos2unix( fs::path p, bool keep_date = false );

}
