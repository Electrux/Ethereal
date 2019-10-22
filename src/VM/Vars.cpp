/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Vars.hpp"

vars_t::vars_t()
	: m_layer( 0 ) {}
vars_t::~vars_t()
{
	for( auto & l : m_vars ) {
		for( auto & v : l.second ) {
			VAR_DREF( v.second );
		}
	}
}

var_base_t * vars_t::get( const std::string & var_name ) const
{
	int layer_iter = m_layer;
	while( layer_iter >= 0 ) {
		if( layer_iter != 0 && m_frozen_till.size() > 0 && layer_iter <= m_frozen_till.back() ) {
			--layer_iter;
			continue;
		}
		if( m_vars.find( layer_iter ) == m_vars.end() ) { --layer_iter; continue; }
		const std::unordered_map< std::string, var_base_t * > & layer = m_vars.at( layer_iter );
		if( layer.find( var_name ) != layer.end() ) return layer.at( var_name );
		--layer_iter;
	}
	return nullptr;
}

bool vars_t::exists( const std::string & var_name ) const
{
	int layer_iter = m_layer;
	while( layer_iter >= 0 ) {
		if( layer_iter != 0 && m_frozen_till.size() > 0 && layer_iter <= m_frozen_till.back() ) {
			--layer_iter;
			continue;
		}
		if( m_vars.find( layer_iter ) == m_vars.end() ) { --layer_iter; continue; }
		const std::unordered_map< std::string, var_base_t * > & layer = m_vars.at( layer_iter );
		if( layer.find( var_name ) != layer.end() ) return true;
		--layer_iter;
	}
	return false;
}

void vars_t::add( const std::string & var_name, var_base_t * const val )
{
	m_vars[ m_layer ][ var_name ] = val;
}

bool vars_t::del( const std::string & var_name, var_base_t ** loc )
{
	int layer_iter = m_layer;
	while( layer_iter >= 0 ) {
		if( layer_iter != 0 && m_frozen_till.size() > 0 && layer_iter <= m_frozen_till.back() ) {
			--layer_iter;
			continue;
		}
		if( m_vars.find( layer_iter ) == m_vars.end() ) { --layer_iter; continue; }
		std::unordered_map< std::string, var_base_t * > & layer = m_vars[ layer_iter ];
		if( layer.find( var_name ) != layer.end() ) {
			if( loc != nullptr ) * loc = layer[ var_name ];
			layer.erase( var_name );
			return true;
		}
		--layer_iter;
	}
	return false;
}

void vars_t::add_scope( const int count ) { m_layer += count; }
void vars_t::pop_scope( std::vector< void * > * locs, const int count )
{
	if( m_layer == 0 ) return;
	for( int i = 0; i < count; ++i ) {
		// First layer (global) cannot be deleted
		if( m_layer == 0 ) break;
		if( m_vars.find( m_layer ) != m_vars.end() ) {
			if( locs != nullptr ) {
				for( auto & vals : m_vars.at( m_layer ) ) {
					locs->push_back( vals.second );
				}
			}
			m_vars.erase( m_layer );
		}
		--m_layer;
	}
}

size_t vars_t::layer_size() const { return m_layer; }