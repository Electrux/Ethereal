/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_map_t::var_map_t() : var_base_t( TYPE_MAP ) {}
var_map_t::var_map_t( std::unordered_map< std::string, var_base_t * > & map )
	: var_base_t( TYPE_MAP ), m_map( map ) {}
var_map_t::~var_map_t()
{
	for( auto & e : m_map ) {
		if( e.second != nullptr ) VAR_DREF( e.second );
	}
}

std::unordered_map< std::string, var_base_t * > & var_map_t::Get() { return m_map; }
std::string var_map_t::type_str() const { return TypeStrs[ TYPE_MAP ]; }
std::string var_map_t::to_str() const
{
	std::string str = "{";
	for( auto it = m_map.begin(); it != m_map.end(); ++it ) {
		str += it->first + ": " + it->second->to_str() + ", ";
	}
	// remove the extra commas
	if( m_map.size() > 0 ) {
		str.pop_back();
		str.pop_back();
	}
	str += "}";
	return str;
}
mpz_class var_map_t::to_int() const { return m_map.size(); }
bool var_map_t::to_bool() const { return m_map.size() > 0; }
var_base_t * var_map_t::copy() const
{
	std::unordered_map< std::string, var_base_t * > map;
	for( auto & e : m_map ) {
		map[ e.first ] = e.second->copy();
	}
	return new var_map_t( map );
}
