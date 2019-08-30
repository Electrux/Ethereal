/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_str_t::var_str_t( const std::string & val, const int parse_ctr )
	: var_base_t( VT_STR, true, parse_ctr ), m_val( val ) {}

std::string var_str_t::to_str() const { return m_val; }
mpz_class var_str_t::to_int() const { return mpz_class( m_val ); }
bool var_str_t::to_bool() const { return !m_val.empty(); }
var_base_t * var_str_t::copy( const int parse_ctr )
	{ return new var_str_t( this->m_val, parse_ctr ); }
void var_str_t::assn( var_base_t * b ) { m_val = AS_STR( b )->get(); }
std::string & var_str_t::get() { return m_val; }