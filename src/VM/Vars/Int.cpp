/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_int_t::var_int_t( const int val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const std::string & val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const bool val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const float val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const mpz_class & val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}

std::string var_int_t::to_str() const { return m_val.get_str(); }
mpz_class var_int_t::to_int() const { return m_val; }
bool var_int_t::to_bool() const { return m_val != 0; }

var_base_t * var_int_t::copy() const { return new var_int_t( this->m_val, this->parse_ctr() ); }
void var_int_t::swap( var_base_t * with )
{
	mpz_class tmp = this->m_val;
	this->m_val = AS_INT( with )->m_val;
	AS_INT( with )->m_val = tmp;
}

mpz_class & var_int_t::get() { return m_val; }