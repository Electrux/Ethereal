/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_vector_t::var_vector_t( const std::vector< var_base_t * > & vec ) : var_base_t( TYPE_VEC ), m_vec( vec ) {}
var_vector_t::~var_vector_t()
{
	for( auto & e : m_vec ) {
		if( e != nullptr ) VAR_DREF( e );
	}
}

std::vector< var_base_t * > & var_vector_t::Get() { return m_vec; }
std::string var_vector_t::type_str() const { return TypeStrs[ TYPE_VEC ]; }
std::string var_vector_t::to_str() const
{
	std::string str = "[";
	for( auto it = m_vec.begin(); it != m_vec.end(); ++it ) {
		if( it == m_vec.end() - 1 ) str += ( * it )->to_str();
		else str += ( * it )->to_str() + ", ";
	}
	str += "]";
	return str;
}
mpz_class var_vector_t::to_int() const { return m_vec.size(); }
bool var_vector_t::to_bool() const { return m_vec.size() > 0; }
var_base_t * var_vector_t::copy() const
{
	std::vector< var_base_t * > vec;
	for( auto & e : m_vec ) {
		vec.push_back( e->copy() );
	}
	return new var_vector_t( vec );
}