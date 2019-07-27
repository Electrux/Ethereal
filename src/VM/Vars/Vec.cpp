/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_vec_t::var_vec_t( std::vector< var_base_t * > & val, const int parse_ctr )
	: var_base_t( VT_VEC, parse_ctr, false ), m_val( val ) {}
var_vec_t::~var_vec_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v );
	}
}
std::string var_vec_t::to_str() const
{
	std::string str = "[";
	for( auto it = m_val.begin(); it != m_val.end(); ++it ) {
		if( it == m_val.end() - 1 ) str += ( * it )->to_str();
		else str += ( * it )->to_str() + ", ";
	}
	str += "]";
	return str;
}
mpz_class var_vec_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_vec_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_vec_t::copy( const int parse_ctr )
{
	std::vector< var_base_t * > newvec;
	for( auto & v : m_val ) {
		newvec.push_back( v->copy( parse_ctr ) );
	}
	return new var_vec_t( newvec, parse_ctr );
}

void var_vec_t::clear()
{
	for( auto & v : m_val ) {
		VAR_DREF( v );
	}
	m_val.clear();
}

std::vector< var_base_t * > & var_vec_t::get() { return m_val; }