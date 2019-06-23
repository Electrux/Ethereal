/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_struct_t::var_struct_t( const std::string & name, const std::vector< std::string > & field_locs,
		      const std::unordered_map< std::string, var_base_t * > fields, const bool is_def )
	: var_base_t( TYPE_STRUCT ), m_name( name ), m_field_locs( field_locs ), m_fields( fields ), m_is_def( is_def ) {}

var_struct_t::~var_struct_t()
{
	for( auto & f : m_fields ) if( f.second != nullptr ) delete f.second;
}

const std::string & var_struct_t::GetName() const { return m_name; }
const std::vector< std::string > & var_struct_t::GetFieldLocs() const { return m_field_locs; }
std::unordered_map< std::string, var_base_t * > & var_struct_t::GetFields() { return m_fields; }

bool var_struct_t::IsDef() const { return m_is_def; }

void var_struct_t::Swap( var_struct_t * var )
{
	if( var == nullptr ) return;
	std::unordered_map< std::string, var_base_t * > tmp_fields = var->m_fields;
	var->m_fields = this->m_fields;
	this->m_fields = tmp_fields;
}

std::string var_struct_t::type_str() const
{
	return std::string( TypeStrs[ TYPE_STRUCT ] ) + "(" + m_name + ")";
}

std::string var_struct_t::to_str() const
{
	std::string res = "@" + m_name + "{";
	for( auto it = m_field_locs.begin(); it != m_field_locs.end(); ++it ) {
		auto & f = * it;
		res += f + " = " + m_fields.at( f )->to_str();
		if( it != m_field_locs.end() - 1 ) {
			res += ", ";
		}
	}
	res += "}";
	return res;
}

mpz_class var_struct_t::to_int() const { return m_field_locs.size(); }

bool var_struct_t::to_bool() const { return true; }

var_base_t * var_struct_t::copy() const
{
	std::unordered_map< std::string, var_base_t * > new_fields;
	for( auto & f : m_fields ) {
		new_fields[ f.first ] = f.second->copy();
	}

	return new var_struct_t( m_name, m_field_locs, new_fields, false );
}
