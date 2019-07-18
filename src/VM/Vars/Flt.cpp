/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_flt_t::var_flt_t( const float val, const int parse_ctr )
	: var_base_t( VT_FLT, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const int val, const int parse_ctr )
	: var_base_t( VT_FLT, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const std::string & val, const int parse_ctr )
	: var_base_t( VT_FLT, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const bool val, const int parse_ctr )
	: var_base_t( VT_FLT, parse_ctr ), m_val( val ) {}
var_flt_t::var_flt_t( const mpf_class & val, const int parse_ctr )
	: var_base_t( VT_FLT, parse_ctr ), m_val( val ) {}

std::string var_flt_t::to_str() const
{
	std::string res;
	mp_exp_t expo;
	res += m_val.get_str( expo );
	if( res.empty() ) res += "0";
	//if( expo - ( int )res.size() == 0 ) return res;
	res += " e^" + std::to_string( expo - ( int )res.size() );
	return res;
}
mpz_class var_flt_t::to_int() const { return mpz_class( m_val ); }
bool var_flt_t::to_bool() const { return m_val != 0.0; }

var_base_t * var_flt_t::copy() const { return new var_flt_t( this->m_val, this->parse_ctr() ); }
void var_flt_t::swap( var_base_t * with )
{
	mpf_class tmp = this->m_val;
	this->m_val = AS_FLT( with )->m_val;
	AS_FLT( with )->m_val = tmp;
}

mpf_class & var_flt_t::get() { return m_val; }