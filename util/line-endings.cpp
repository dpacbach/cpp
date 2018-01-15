/****************************************************************
* Utilities for handling line endings
****************************************************************/
#include "line-endings.hpp"
#include "io.hpp"

using namespace std;

namespace util {

// Open the given path and edit  it to remove all 0x0D characters.
// This  attempts to emulate the command line utility of the same
// name. As with  the  command,  the  `keep_date`  flag indicates
// whether the timestamp on the  file  should remain unchanged in
// the event that the  file  contains  no  0x0D characters. By de-
// fault, the timestamp will always be touched.
void dos2unix( fs::path p, bool keep_date ) {

    auto v    = read_file( p );
    auto size = v.size();

    dos2unix( v );

    // We can use the size of the  new vector relative to the old
    // size to determine whether  it  was  changed (i.e., whether
    // there were any 0x0D  characters  in  it), but the dos2unix
    // function  will  only ever edit the vector by removing char-
    // acters.
    if( v.size() == size ) {
        // There were no 0x0D characters;  vector  unchanged.  So
        // therefore  whether  or  not  the timestamp of the file
        // gets touched is determined by the caller's  preference.
        if( !keep_date )
            // In  this case we could just fall through and write
            // the file, which would touch the  time  stamp,  but
            // this seems more efficient and is (hopefully)
            // equivalent.
            touch( p );
        return;
    }

    // Some 0x0D characters were  found  and  removed, so rewrite
    // the  file;  new  file size will be smaller than before and
    // time stamp will be touched.
    write_file( p, v );
}

}
