/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <iomanip>
#include <sstream>

#include "Base.hpp"

var_flt_t::var_flt_t( const float val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_FLT, true, src_idx, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const int val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_FLT, true, src_idx, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const std::string & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_FLT, true, src_idx, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const bool val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_FLT, true, src_idx, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const mpfr::mpreal & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_FLT, true, src_idx, parse_ctr ), m_val( val ) {}

std::string var_flt_t::to_str() const
{
	std::ostringstream oss;
	oss << std::setprecision( get_float_precision() + 1 ) << m_val;
	return oss.str();
}
mpz_class var_flt_t::to_int() const { return mpz_class( mpf_class( m_val.toString() ) ); }
bool var_flt_t::to_bool() const { return m_val != 0.0; }

var_base_t * var_flt_t::copy( const int src_idx, const int parse_ctr )
	{ return new var_flt_t( this->m_val, src_idx, parse_ctr ); }
void var_flt_t::assn( var_base_t * b ) { m_val = AS_FLT( b )->get(); }
mpfr::mpreal & var_flt_t::get() { return m_val; }