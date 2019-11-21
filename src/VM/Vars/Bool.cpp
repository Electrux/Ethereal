/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_bool_t::var_bool_t( const int val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_BOOL, true, src_idx, parse_ctr ), m_val( val ) {}
var_bool_t::var_bool_t( const float val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_BOOL, true, src_idx, parse_ctr ), m_val( val ) {}
var_bool_t::var_bool_t( const std::string & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_BOOL, true, src_idx, parse_ctr ), m_val( val != "0" && val != "" ) {}
var_bool_t::var_bool_t( const bool val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_BOOL, true, src_idx, parse_ctr ), m_val( val ) {}

std::string var_bool_t::to_str() const { return m_val ? "true" : "false"; }
mpz_class var_bool_t::to_int() const { return mpz_class( m_val ); }
bool var_bool_t::to_bool() const { return m_val; }

var_base_t * var_bool_t::copy( const int src_idx, const int parse_ctr )
	{ return new var_bool_t( this->m_val, src_idx, parse_ctr ); }
void var_bool_t::assn( var_base_t * b ) { m_val = AS_BOOL( b )->get(); }
bool & var_bool_t::get() { return m_val; }