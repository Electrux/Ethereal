/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_struct_def_t::var_struct_def_t( const std::string & name, std::vector< std::string > & pos,
				    std::unordered_map< std::string, var_base_t * > & val,
				    const int src_idx, const int parse_ctr )
	: var_base_t( VT_STRUCT, false, src_idx, parse_ctr ), m_name( name ), m_pos( pos ), m_val( val ) {}
var_struct_def_t::~var_struct_def_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
}
std::string var_struct_def_t::type_str() const { return m_name; }
std::string var_struct_def_t::to_str() const
{
	std::string str = m_name + "{";
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
mpz_class var_struct_def_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_struct_def_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_struct_def_t::copy( const int src_idx, const int parse_ctr )
{
	std::unordered_map< std::string, var_base_t * > newmap;
	for( auto & v : m_val ) {
		newmap[ v.first ] = v.second->copy( src_idx, parse_ctr );
	}
	return new var_struct_t( m_name, newmap, src_idx, parse_ctr );
}

std::string & var_struct_def_t::get_name() { return m_name; }
std::vector< std::string > & var_struct_def_t::get_pos() { return m_pos; }
std::unordered_map< std::string, var_base_t * > & var_struct_def_t::get_val() { return m_val; }