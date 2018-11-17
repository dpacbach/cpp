/****************************************************************
* SVN-status Utilities
****************************************************************/
#pragma once

#include "bimap.hpp"
#include "fs.hpp"
#include "string-util.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace svn {

struct Status {

    // There are many change types that can be  supplied  by  SVN,
    // but these are the main ones that we're usually  interested
    // in. Anything entries with change types that we're  not  in-
    // terested in are ignored.
    enum class ChangeType {
        /* M */ modified,
        /* A */ added,
        /* D */ deleted,
        /* C */ conflicted,
        /* ? */ unversioned
    };

    // This path will be exactly as it appears in the  XML,  with
    // the  exception  that  any backslashes will be converted to
    // forwarded slashes.
    fs::path     path;

    ChangeType   change;

    // This flag is needed because the ChangeType does not always
    // yield  `conflicted` in the case of a tree conflict; to SVN
    // this is somehow different.
    bool         tree_conflict;
};

using CTMap = util::BiMapFixed<std::string,Status::ChangeType>;
// Construct a global  map  to  change  between  enums  and their
// string representations.
extern CTMap g_change;

// Take a string holding  the  output  of  the `svn status --xml`
// command and parse it,  then  extract  enough information to re-
// turn a list Status descriptors. Throws  on  error  or  if  the
// structure of the xml is not exactly as expected.
std::vector<Status> parse_status_xml( std::string const& xml );
std::vector<Status> parse_status_xml( std::istream&      in  );

bool operator< ( Status const& lhs, Status const& rhs );
bool operator==( Status const& lhs, Status const& rhs );

// For convenience.
std::ostream& operator<<( std::ostream& out,
                          Status const& status );
std::ostream& operator<<( std::ostream& out,
                          Status::ChangeType const& ct );

} // namespace svn

namespace util {

template<>
std::string to_string( svn::Status const& s );

template<>
std::string to_string( svn::Status::ChangeType const& s );

} // namespace util
