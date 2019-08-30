/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_struct_t::var_struct_t( const std::string & name, std::unordered_map< std::string, var_base_t * > & val, const int parse_ctr )
	: var_base_t( VT_STRUCT, true, parse_ctr ), m_name( name ), m_val( val ) {}
var_struct_t::~var_struct_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
}
std::string var_struct_t::type_str() const { return m_name; }
std::string var_struct_t::to_str() const
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
mpz_class var_struct_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_struct_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_struct_t::copy( const int parse_ctr )
{
	std::unordered_map< std::string, var_base_t * > newmap;
	for( auto & v : m_val ) {
		newmap[ v.first ] = v.second->copy( parse_ctr );
	}
	return new var_struct_t( m_name, newmap, parse_ctr );
}

void var_struct_t::clear()
{
	for( auto & v : m_val ) {
		VAR_DREF( v.second );
	}
	m_val.clear();
}

std::string & var_struct_t::get_name() { return m_name; }
std::unordered_map< std::string, var_base_t * > & var_struct_t::get_val() { return m_val; }

void var_struct_t::assn( var_base_t * b )
{
	this->clear();
	var_struct_t * bt = static_cast< var_struct_t * >( b );
	for( auto & x : bt->m_val ) {
		this->m_val[ x.first ] = x.second->copy( b->parse_ctr() );
	}
}