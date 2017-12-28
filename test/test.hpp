/****************************************************************
* Unit test infrastructure
****************************************************************/
#pragma once

#include "colors.hpp"
#include "macros.hpp"

#include <iomanip>
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

#define TRUE_( a )                                        \
    if( !(a) ) {                                          \
        cerr << " ==> " << testing::failed() << endl;     \
        cerr << testing::bar() << endl;                   \
        cerr << "On line " << TO_STRING( __LINE__ ) ": "; \
        cerr << "assert " << #a << endl;                  \
        cerr << testing::bar() << endl;                   \
        throw logic_error( "test failed." );              \
    }

// a  is an expression that must be true, and b is just something
// printable that will be displayed  in  the  event  a  is  false.
#define TRUE( a, b )                                      \
    if( !(a) ) {                                          \
        cerr << " ==> " << testing::failed() << endl;     \
        cerr << testing::bar() << endl;                   \
        cerr << "On line " << TO_STRING( __LINE__ ) ": "; \
        cerr << b << endl;                                \
        cerr << testing::bar() << endl;                   \
        throw logic_error( "test failed." );              \
    }

#define EQUALS( a, b )                                         \
    if( !((a) == (b)) ) {                                      \
        cerr << " ==> " << testing::failed() << endl;          \
        cerr << testing::bar() << endl;                        \
        cerr << "On line " << TO_STRING( __LINE__ ) ": " << #a \
             << " != " << #b << endl;                          \
        cerr << "instead got: " << (a);                        \
        cerr << " ?= " << #b << endl;                          \
        cerr << testing::bar() << endl;                        \
        throw logic_error( "test failed." );                   \
    }

#define THROWS( a )                                 \
    bool STRING_JOIN( __threw_, __LINE__ ) = false; \
    try {                                           \
        a;                                          \
    } catch( std::exception const& ) {              \
        STRING_JOIN( __threw_, __LINE__ ) = true;   \
    }                                               \
    TRUE( STRING_JOIN( __threw_, __LINE__ ),        \
          "expression: " #a " did not throw." );

// This is used to create a unit test function.
#define TEST( a )                                         \
    void STRING_JOIN( __test_, a )();                     \
    void STRING_JOIN( test_, a )() {                      \
        string test = string( "Test " ) + TO_STRING( a ); \
        cout << left << setw( 40 ) << test;               \
        STRING_JOIN( __test_, a )();                      \
        cout << " ==> " << testing::passed();             \
        cout << util::c_norm << endl;                     \
    }                                                     \
    void STRING_JOIN( __test_, a )()
