/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_map_t::var_map_t( std::unordered_map< std::string, var_base_t * > & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_MAP, true, src_idx, parse_ctr ), m_val( val ) {}
var_map_t::~var_map_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
}
std::string var_map_t::to_str() const
{
	std::string str = "{";
	for( auto it = m_val.begin(); it != m_val.end(); ++it ) {
		str += it->first + ": " + it->second->to_str() + ", ";
	}
	// remove the extra commas
	if( m_val.size() > 0 ) {
		str.pop_back();
		str.pop_back();
	}
	str += "}";
	return str;
}
mpz_class var_map_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_map_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_map_t::copy( const int src_idx, const int parse_ctr )
{
	std::unordered_map< std::string, var_base_t * > newmap;
	for( auto & v : m_val ) {
		newmap[ v.first ] = v.second->copy( src_idx, parse_ctr );
	}
	return new var_map_t( newmap, src_idx, parse_ctr );
}

void var_map_t::clear()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
	m_val.clear();
}

std::unordered_map< std::string, var_base_t * > & var_map_t::get() { return m_val; }

void var_map_t::assn( var_base_t * b )
{
	this->clear();
	var_map_t * bt = static_cast< var_map_t * >( b );
	for( auto & x : bt->m_val ) {
		this->m_val[ x.first ] = x.second->copy( b->src_idx(), b->parse_ctr() );
	}
}