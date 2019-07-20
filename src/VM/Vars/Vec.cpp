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
	: var_base_t( VT_VEC, parse_ctr ), m_val( val ) {}
var_vec_t::~var_vec_t()
{
	for( auto & v : m_val ) {
		VAR_DREF( v );
	}
}
std::string var_vec_t::to_str() const { return "vec(" + std::to_string( m_val.size() ) + ")"; }
mpz_class var_vec_t::to_int() const { return mpz_class( m_val.size() ); }
bool var_vec_t::to_bool() const { return m_val.size() > 0; }

var_base_t * var_vec_t::copy( const int parse_ctr ) const
{
	std::vector< var_base_t * > newvec;
	for( auto & v : m_val ) {
		newvec.push_back( v->copy( parse_ctr ) );
	}
	return new var_vec_t( newvec, parse_ctr );
}

std::vector< var_base_t * > & var_vec_t::get() { return m_val; }