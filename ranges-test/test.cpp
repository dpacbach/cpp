
#include <range/v3/all.hpp>

#include <vector>
#include <iostream>

#include <stdio.h>

using namespace std;

using namespace ranges;

template<typename T, typename U>
ostream& operator<<( ostream& out, pair<T, U> const& t ) {
    return (out << "(" << get<0>( t ) << ", " << get<1>( t ) << ")"); 
}

#define FOR_EACH( b, a ) \
    for( auto b = a.begin(); b != a.end(); ++b )

int main() {

    auto v = vector{ 1, 6, 2, 5, 4, 3, 11, 6, 12, 4, 8 };

    ranges::sort( v );

    auto inc     = []( auto i ){ return i+1; };
    auto is_even = []( auto i ){ return i%2==0; };

    auto rng = v | view::transform( inc )
                 | view::remove_if( is_even );

    auto zipped = view::zip( view::iota( 0 ), rng );

    /*
    FOR_EACH( i_pair, zipped )
        cout << *i_pair << endl;

    for( auto i = zipped.begin(); i != zipped.end(); ++i )
        cout << (*i) << endl;
    */

    ranges::for_each( zipped, []( auto const& p ) {
        cout << p << endl;
    } );
}
