/****************************************************************
* Graphs
****************************************************************/
#pragma once

#include "macros.hpp"
#include "non-copyable.hpp"
#include "bimap.hpp"

#include <map>
#include <unordered_map>
#include <vector>

namespace util {

/****************************************************************
* Directed Graph (not acyclic)
****************************************************************/
template<typename NameT>
class DirectedGraph : util::non_copyable {

public:

    using Id       = size_t;
    using Edges    = std::vector<Id>;
    using GraphVec = std::vector<Edges>;
    using NamesMap = BDIndexMap<NameT>;

    DirectedGraph( std::map<
                       NameT,
                       std::vector<NameT>
                   > const& m );

    DirectedGraph( std::unordered_map<
                       NameT,
                       std::vector<NameT>
                   > const& m );

    DirectedGraph( GraphVec&& edges,
                   NamesMap&& names );

    std::vector<NameT> accessible( NameT const& name ) const;

private:

    NamesMap m_names;
    GraphVec m_edges;

};

template<typename NameT>
DirectedGraph<NameT>::DirectedGraph( GraphVec&& edges,
                                     NamesMap&& names )
    : m_names( std::move( names ) ),
      m_edges( std::move( edges ) )
{
    ASSERT_( m_names.size() == m_edges.size() );
}

template<typename NameT>
DirectedGraph<NameT>::DirectedGraph( std::map<
                                         NameT,
                                         std::vector<NameT>
                                     > const& m )
      : m_names( {} ),
        m_edges( {} ) {

    std::vector<NameT> names; names.reserve( m.size() );
    for( auto const& p : m )
        names.push_back( p.first );
    // true == items are sorted, because we are using a map.
    m_names = BDIndexMap( std::move( names ), true );

    GraphVec edges; edges.reserve( m.size() );
    for( auto const& p : m ) {
        Edges ids; ids.reserve( p.second.size() );
        for( auto const& v : p.second ) {
            auto key = m_names.key( v );
            ASSERT_( key );
            ids.push_back( *key );
        }
        edges.emplace_back( std::move( ids ) );
    }
    m_edges = std::move( edges );
}

template<typename NameT>
std::vector<NameT>
DirectedGraph<NameT>::accessible( NameT const& name ) const {
    std::vector<NameT> res;
    std::vector<Id>    visited( m_names.size(), 0 );
    std::vector<Id>    to_visit;

    auto start = m_names.key( name );
    if( start )
        to_visit.push_back( *start );
     
    while( to_visit.size() ) {
        Id i = to_visit.back(); to_visit.pop_back();
        if( visited[i] )
            // We  may have duplicates in the stack if we include
            // some  thing  a  second  time  before the first one
            // (which is already in the stack) is visited.
            continue;
        visited[i] = 1;
        auto name = m_names.val( i );
        ASSERT_( name );
        res.push_back( *name );
        for( Id i : m_edges[i] )
            if( !visited[i] )
                to_visit.push_back( i );
    }

    return res;
}

} // namespace util
