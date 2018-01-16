/****************************************************************
* Utilities for handling line endings
****************************************************************/
#include "line-endings.hpp"
#include "io.hpp"

using namespace std;

namespace util {

namespace {

// Functions that change line endings just take a vector  and  mu-
// tate it, which is fine for most use cases.
using Changer = void( vector<char>& );

// Will read in the contents of  the  file (which must exist) and
// apply the function to it to change line endings, then write it
// back  out. The keep_date flag is used to determine whether the
// timestamp on the file should always be touched (false)  or  if
// it should only be touched  in  the  contents  of the file were
// modified  (true). Bool return value indicates whether file con-
// tents were changed or not (regardless of timestamp).
bool change_le( Changer* f, fs::path p, bool keep_date ) {

    auto v    = read_file( p );
    auto size = v.size();

    // This function should mutate the vector.
    f( v );

    // We can use the size of the  new vector relative to the old
    // size to determine whether  it  was  changed (i.e., whether
    // any line endings were changed); i.e., a line ending change
    // will leave the size unchanged if and only if there were no
    // line endings that needed to be  changed, in which case the
    // contents of  the  buffer  in  general  will  be  unchanged.
    if( v.size() == size ) {
        // There  were  some  line  endings changes, so therefore
        // whether  or not the timestamp of the file gets touched
        // is determined by the caller's preference.
        if( !keep_date )
            // In  this case we could just fall through and write
            // the file, which would touch the  time  stamp,  but
            // this seems more efficient and is (hopefully)
            // equivalent.
            touch( p );
        return false;
    }

    // Some  line  endings were changed, so rewrite the file; new
    // file size will be different at this point.
    write_file( p, v );
    return true;
}

} // anonymous namespace

// Open the given path and edit  it to remove all 0x0D characters.
// This  attempts to emulate the command line utility of the same
// name. As with  the  command,  the  `keep_date`  flag indicates
// whether the timestamp on the  file  should remain unchanged in
// the event that the  file  contains  no  0x0D characters. By de-
// fault, the timestamp will always be touched. Bool return value
// indicates whether file contents  were  changed  or not (regard-
// less of timestamp).
bool dos2unix( fs::path p, bool keep_date ) {
    return change_le( dos2unix, p, keep_date );
}

// Open  the given path and edit it to change LF to CRLF. This at-
// tempts to emulate the command line utility of  the  same  name.
// As with the command, the `keep_date`  flag  indicates  whether
// the timestamp on the file should remain unchanged in the event
// that  the  file already contains only CRLF line endings. By de-
// fault, the timestamp  will  always  be  touched.  NOTE: if the
// input container does not contain valid unix (e.g., if  it  con-
// tains solitary CR's) then the output may not contain valid DOS
// line endings. Bool return value  indicates  whether  file  con-
// tents were changed or not (regardless of timestamp).
bool unix2dos( fs::path p, bool keep_date ) {
    return change_le( unix2dos, p, keep_date );
}

}
