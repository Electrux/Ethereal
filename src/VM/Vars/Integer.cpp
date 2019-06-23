/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_int_t::var_int_t( const mpz_class & val ) : var_base_t( TYPE_INT ), m_val( val ) {}
var_int_t::var_int_t( const std::string & val ) : var_base_t( TYPE_INT ), m_val( val ) {}
var_int_t::~var_int_t() {}

mpz_class & var_int_t::Get() { return m_val; }
std::string var_int_t::type_str() const { return TypeStrs[ TYPE_INT ]; }
std::string var_int_t::to_str() const { return m_val.get_str(); }
mpz_class var_int_t::to_int() const { return m_val; }
bool var_int_t::to_bool() const { return m_val != 0; }
var_base_t * var_int_t::copy() const { return new var_int_t( m_val ); }