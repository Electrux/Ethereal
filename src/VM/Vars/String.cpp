/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

static void EvalStr( std::string & str );

var_str_t::var_str_t( const std::string & val, const bool from_subscr_load )
	: var_base_t( TYPE_STR ), m_val( val ), m_from_subscr_load( from_subscr_load ) { EvalStr( m_val ); }
var_str_t::~var_str_t() {}

std::string & var_str_t::Get() { return m_val; }
bool var_str_t::FromSubscrLoad() { return m_from_subscr_load; }
std::string var_str_t::type_str() const { return TypeStrs[ TYPE_STR ]; }
std::string var_str_t::to_str() const { return m_val; }
mpz_class var_str_t::to_int() const
{
	if( !is_num( m_val ) ) return mpz_class( "NaN" );
	return mpz_class( m_val );
}
bool var_str_t::to_bool() const { return m_val != "false" && m_val != "0"; }
var_base_t * var_str_t::copy() const { return new var_str_t( m_val, m_from_subscr_load ); }

static void EvalStr( std::string & str )
{
	for( auto it = str.begin(); it != str.end(); ++it ) {
		if( * it == '\\' ) {
			if( it + 1 >= str.end() ) continue;
			it = str.erase( it );
			if( * it == 'a' ) * it = '\a';
			else if( * it == 'b' ) * it = '\b';
			else if( * it == 'f' ) * it = '\f';
			else if( * it == 'n' ) * it = '\n';
			else if( * it == 'r' ) * it = '\r';
			else if( * it == 't' ) * it = '\t';
			else if( * it == 'v' ) * it = '\v';
		}
	}
}
