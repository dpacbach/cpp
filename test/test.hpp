/****************************************************************
* Unit test infrastructure
****************************************************************/
#pragma once

#include "colors.hpp"
#include "macros.hpp"

#include <iomanip>
#include <sstream>
#include <string>

namespace testing {

std::string failed() {
    return util::c_red + "Failed" + util::c_norm;
}

std::string passed() {
    return util::c_green + "Passed" + util::c_norm;
}

std::string bar() {
    return "---------------------------------------------------";
}

} // namespace testing

#define TRUE_( a )                                       \
    if( !(a) ) {                                         \
        using util::operator<<;                          \
        ostringstream ss;                                \
        ss << testing::bar() << "\n";                    \
        ss << "On line " << TO_STRING( __LINE__ ) ": ";  \
        ss << "assert " << #a << "\n";                   \
        ss << testing::bar() << "\n";                    \
        throw logic_error( ss.str() );                   \
    }

// a  is an expression that must be true, and b is just something
// printable that will be displayed  in  the  event  a  is  false.
#define TRUE( a, b )                                     \
    if( !(a) ) {                                         \
        using util::operator<<;                          \
        ostringstream ss;                                \
        ss << testing::bar() << "\n";                    \
        ss << "On line " << TO_STRING( __LINE__ ) ": ";  \
        ss << b << "\n";                                 \
        ss << testing::bar() << "\n";                    \
        throw logic_error( ss.str() );                   \
    }

#define EQUALS( a, b )                                        \
    if( !((a) == (b)) ) {                                     \
        using util::operator<<;                               \
        ostringstream ss;                                     \
        ss << testing::bar() << "\n";                         \
        ss << "On line " << TO_STRING( __LINE__ ) ": " << #a  \
             << " != " << #b << "\n";                         \
        ss << "instead got: " << (a);                         \
        ss << " ?= " << #b << "\n";                           \
        ss << testing::bar() << "\n";                         \
        throw logic_error( ss.str() );                        \
    }

#define THROWS( a )                                  \
    bool STRING_JOIN( __threw_, __LINE__ ) = false;  \
    try {                                            \
        a;                                           \
    } catch( std::exception const& ) {               \
        STRING_JOIN( __threw_, __LINE__ ) = true;    \
    }                                                \
    TRUE( STRING_JOIN( __threw_, __LINE__ ),         \
          "expression: " #a " did not throw." );

// This is used to create a unit test function.
#define TEST( a )                                          \
    void STRING_JOIN( __test_, a )();                      \
    void STRING_JOIN( test_, a )() {                       \
        using util::operator<<;                            \
        string test = string( "Test " ) + TO_STRING( a );  \
        cout << left << setw( 40 ) << test;                \
        bool threw = false;                                \
        string err;                                        \
        try {                                              \
            STRING_JOIN( __test_, a )();                   \
        } catch( std::exception const& e ) {               \
            err = e.what();                                \
            threw = true;                                  \
        } catch( ... ) {                                   \
            err = "unknown exception";                     \
            threw = true;                                  \
        }                                                  \
        if( threw ) {                                      \
            cerr << " ==> " << testing::failed() << "\n";  \
            cerr << err << "\n";                           \
            throw runtime_error( "test failed" );          \
        } else {                                           \
            cout << " ==> " << testing::passed();          \
            cout << util::c_norm << "\n";                  \
        }                                                  \
    }                                                      \
    void STRING_JOIN( __test_, a )()
