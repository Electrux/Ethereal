/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_enum_t::var_enum_t( const std::string & name, std::unordered_map< std::string, var_int_t * > & val,
			const int parse_ctr )
	: var_base_t( VT_ENUM, false, parse_ctr ), m_name( name ), m_val( val ) {}
var_enum_t::~var_enum_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
}
std::string var_enum_t::to_str() const { return "enum:" + m_name + "(" + std::to_string( m_val.size() ) + ")"; }
mpz_class var_enum_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_enum_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_enum_t::copy( const int parse_ctr )
{
	std::unordered_map< std::string, var_int_t * > newmap;
	for( auto & v : m_val ) {
		newmap[ v.first ] = AS_INT( v.second->copy( parse_ctr ) );
	}
	return new var_enum_t( this->m_name, newmap, parse_ctr );
}
std::string var_enum_t::get_name() { return m_name; }
std::unordered_map< std::string, var_int_t * > & var_enum_t::get_val() { return m_val; }