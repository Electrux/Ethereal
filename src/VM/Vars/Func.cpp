/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_func_t::var_func_t( const std::string & name, var_vector_t * args, var_block_t * body )
	: var_base_t( TYPE_FUNC ), m_name( name ), m_args( args ), m_body( body )
{}

var_func_t::var_func_t( var_vector_t * args, var_block_t * body )
	: var_base_t( TYPE_FUNC ), m_args( args ), m_body( body )
{}
var_func_t::~var_func_t()
{
	if( m_args != nullptr ) delete m_args;
	if( m_body != nullptr ) delete m_body;
}

void var_func_t::set_name( const std::string & name ) { m_name = name; }
const std::string & var_func_t::get_name() { return m_name; }
var_vector_t * var_func_t::get_args() { return m_args; }
var_block_t * var_func_t::get_body() { return m_body; }

std::string var_func_t::type_str() const { return std::string( TypeStrs[ TYPE_FUNC ] ) + "(" + m_name + ")"; }

std::string var_func_t::to_str() const { return m_name; }
mpz_class var_func_t::to_int() const { return m_args->to_int(); }
bool var_func_t::to_bool() const { return m_args->to_int() > 0; }
var_base_t * var_func_t::copy() const
{
	return new var_func_t( m_name,
			       ( var_vector_t * )m_args->copy(),
			       ( var_block_t * )m_body->copy() );
}
