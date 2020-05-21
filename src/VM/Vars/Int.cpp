/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_int_t::var_int_t( const int val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const size_t val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const std::string & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const bool val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const float val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const mpz_class & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const mpfr::mpreal & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_INT, true, src_idx, parse_ctr ), m_val( mpf_class( val.toString() ) ) {}

std::string var_int_t::to_str() const { return m_val.get_str(); }
mpz_class var_int_t::to_int() const { return m_val; }
bool var_int_t::to_bool() const { return m_val != 0; }

var_base_t * var_int_t::copy( const int src_idx, const int parse_ctr )
	{ return new var_int_t( this->m_val, src_idx, parse_ctr ); }
void var_int_t::assn( var_base_t * b ) { m_val = AS_INT( b )->get(); }
mpz_class & var_int_t::get() { return m_val; }