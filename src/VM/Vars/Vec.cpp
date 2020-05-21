/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_vec_t::var_vec_t( const std::vector< var_base_t * > & val, const int src_idx, const int parse_ctr, const bool is_var_arg )
	: var_base_t( VT_VEC, true, src_idx, parse_ctr ), m_val( val ), m_is_var_arg( is_var_arg ) {}
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

var_base_t * var_vec_t::copy( const int src_idx, const int parse_ctr )
{
	std::vector< var_base_t * > newvec;
	for( auto & v : m_val ) {
		newvec.push_back( v->copy( src_idx, parse_ctr ) );
	}
	return new var_vec_t( newvec, src_idx, parse_ctr, m_is_var_arg );
}

void var_vec_t::clear()
{
	for( auto & v : m_val ) {
		VAR_DREF( v );
	}
	m_val.clear();
}

std::vector< var_base_t * > & var_vec_t::get() { return m_val; }

void var_vec_t::assn( var_base_t * b )
{
	this->clear();
	var_vec_t * bt = static_cast< var_vec_t * >( b );
	for( auto & x : bt->m_val ) {
		this->m_val.push_back( x->copy( b->src_idx(), b->parse_ctr() ) );
	}
}