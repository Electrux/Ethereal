/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_bool_t::var_bool_t( const bool & val ) : var_base_t( TYPE_BOOL ), m_val( val ) {}
var_bool_t::var_bool_t( const std::string & val ) : var_base_t( TYPE_BOOL ) { m_val = val == "true" || ( val != "0" && val != "false" ); }
var_bool_t::~var_bool_t() {}

bool & var_bool_t::Get() { return m_val; }
std::string var_bool_t::type_str() const { return TypeStrs[ TYPE_BOOL ]; }
std::string var_bool_t::to_str() const { return m_val ? "true" : "false"; }
mpz_class var_bool_t::to_int() const { return m_val ? 1 : 0; }
bool var_bool_t::to_bool() const { return m_val; }
var_base_t * var_bool_t::copy() const { return new var_bool_t( m_val ); }