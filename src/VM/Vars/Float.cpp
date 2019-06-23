/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_flt_t::var_flt_t( const mpf_class & val ) : var_base_t( TYPE_FLT ), m_val( val ) {}
var_flt_t::var_flt_t( const std::string & val ) : var_base_t( TYPE_FLT ), m_val( val ) {}
var_flt_t::~var_flt_t() {}

mpf_class & var_flt_t::Get() { return m_val; }
std::string var_flt_t::type_str() const { return TypeStrs[ TYPE_FLT ]; }
std::string var_flt_t::to_str() const {
	std::string res;
	mp_exp_t expo;
	res += m_val.get_str( expo );
	if( res.empty() ) res += "0";
	//if( expo - ( int )res.size() == 0 ) return res;
	res += " e^" + std::to_string( expo - ( int )res.size() );
	return res;
}
mpz_class var_flt_t::to_int() const { return mpz_class( m_val ); }
bool var_flt_t::to_bool() const { return m_val != 0; }
var_base_t * var_flt_t::copy() const { return new var_flt_t( m_val ); }