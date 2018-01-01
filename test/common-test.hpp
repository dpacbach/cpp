/****************************************************************
* Unit test infrastructure
****************************************************************/
#pragma once

#include "colors.hpp"
#include "macros.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace testing {

// This  is the type of a unit test function. All that it does is
// to  throw an exception if the test fails. If it returns, it is
// assumed to have passed.
using TestType = void(void);

// Global test list: tests are automatically registered and added
// to this list at static initialization time.
std::vector<TestType*>& test_list();

void run_all_tests();

// Functions for printing to console
std::string fail();
std::string pass();
std::string bar();

} // namespace testing

#define TRUE_( a )                                       \
    if( !(a) ) {                                         \
        using util::operator<<;                          \
        ostringstream ss;                                \
        ss << "On line " << TO_STRING( __LINE__ ) ": ";  \
        ss << "assert " << #a;                           \
        throw logic_error( ss.str() );                   \
    }

// a  is an expression that must be true, and b is just something
// printable that will be displayed  in  the  event  a  is  false.
#define TRUE( a, b )                                     \
    if( !(a) ) {                                         \
        using util::operator<<;                          \
        ostringstream ss;                                \
        ss << "On line " << TO_STRING( __LINE__ ) ": ";  \
        ss << b;                                         \
        throw logic_error( ss.str() );                   \
    }

#define EQUALS( a, b )                                        \
    if( !((a) == (b)) ) {                                     \
        using util::operator<<;                               \
        ostringstream ss;                                     \
        ss << "On line " << TO_STRING( __LINE__ ) ": " << #a  \
             << " != " << #b << "\n";                         \
        ss << "instead got: " << (a);                         \
        ss << " ?= " << #b;                                   \
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
            cerr << " | " << testing::fail() << "\n";      \
            cerr << testing::bar() << "\n";                \
            cerr << err << "\n";                           \
            cerr << testing::bar() << "\n";                \
            throw runtime_error( "test failed" );          \
        } else {                                           \
            cout << " | " << testing::pass();              \
            cout << util::c_norm << "\n";                  \
        }                                                  \
    }                                                      \
    STARTUP() {                                            \
        test_list().push_back( STRING_JOIN( test_, a ) );  \
    }                                                      \
    void STRING_JOIN( __test_, a )()
