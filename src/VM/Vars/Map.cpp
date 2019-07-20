/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_map_t::var_map_t( std::unordered_map< std::string, var_base_t * > & val, const int parse_ctr )
	: var_base_t( VT_MAP, parse_ctr ), m_val( val ) {}
var_map_t::~var_map_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
}
std::string var_map_t::to_str() const { return "map(" + std::to_string( m_val.size() ) + ")"; }
mpz_class var_map_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_map_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_map_t::copy( const int parse_ctr ) const
{
	std::unordered_map< std::string, var_base_t * > newmap;
	for( auto & v : m_val ) {
		newmap[ v.first ] = v.second->copy( parse_ctr );
	}
	return new var_map_t( newmap, parse_ctr );
}

std::unordered_map< std::string, var_base_t * > & var_map_t::get() { return m_val; }