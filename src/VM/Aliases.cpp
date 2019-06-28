/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Aliases.hpp"

aliases_t::aliases_t()
{
	m_aliases[ "__default__" ] = {};
}

aliases_t::~aliases_t() {}

const std::vector< std::string > & aliases_t::get( const std::string & alias_name ) const
{
	if( m_aliases.find( alias_name ) != m_aliases.end() ) return m_aliases.at( alias_name );
	return m_aliases.at( "__default__" );
}

bool aliases_t::exists( const std::string & alias_name ) const
{
	return m_aliases.find( alias_name ) != m_aliases.end();
}

void aliases_t::add( const std::string & alias_name, const std::vector< std::string > & alias )
{
	m_aliases[ alias_name ] = alias;
}

bool aliases_t::del( const std::string & alias_name )
{
	if( m_aliases.find( alias_name ) != m_aliases.end() ) {
		m_aliases.erase( alias_name );
		return true;
	}
	return false;
}

size_t aliases_t::count() const { return m_aliases.size(); }