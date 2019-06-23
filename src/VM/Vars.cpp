/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Vars/Base.hpp"
#include "Vars.hpp"

vars_t::vars_t() : m_layer( 0 ) {}
vars_t::~vars_t()
{
	for( int i = m_layer; i >= 0; --i ) {
		auto loc = m_vars[ i ];
		for( auto & l : loc ) {
#ifdef DEBUG_MODE
			fprintf( stdout, "Deleting variable: %s with reference count: %d\n",
				 l.first.c_str(), l.second->ref_count() );
#endif
			VAR_DREF( l.second );
		}
	}
}

var_base_t * vars_t::get_var( const std::string & var_name ) const
{
	int layer_iter = m_layer;
	while( layer_iter >= 0 ) {
		if( m_vars.find( layer_iter ) == m_vars.end() ) { --layer_iter; continue; }
		const std::unordered_map< std::string, var_base_t * > & layer = m_vars.at( layer_iter );
		if( layer.find( var_name ) != layer.end() ) return layer.at( var_name );
		--layer_iter;
	}
	return nullptr;
}

bool vars_t::var_exists( const std::string & var_name, const bool all_layers ) const
{
	int layer_iter = m_layer;
	while( layer_iter >= 0 ) {
		if( m_vars.find( layer_iter ) == m_vars.end() ) { --layer_iter; continue; }
		const std::unordered_map< std::string, var_base_t * > & layer = m_vars.at( layer_iter );
		if( layer.find( var_name ) != layer.end() ) return true;
		--layer_iter;
		if( !all_layers ) break;
	}
	return false;
}

void vars_t::add_var( const std::string & var_name, var_base_t * const val )
{
	m_vars[ m_layer ][ var_name ] = val;
}

bool vars_t::del_var( const std::string & var_name, var_base_t ** loc )
{
	int layer_iter = m_layer;
	while( layer_iter >= 0 ) {
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

void vars_t::add_layer( const int count ) { m_layer += count; }
void vars_t::pop_layer( std::vector< void * > * locs, const int count )
{
	if( m_layer == 0 ) return;
	for( int i = 0; i < count; ++i ) {
		// First layer (global) cannot be deleted
		if( m_layer == 0 ) break;
		if( m_vars.find( m_layer ) != m_vars.end() ) {
			if( locs != nullptr ) {
				for( auto & vals : m_vars.at( m_layer ) ) {
					locs->push_back( vals.second );
#ifdef DEBUG_MODE
					fprintf( stdout, "Deleting variable: %s with reference count: %d\n",
						 vals.first.c_str(), vals.second->ref_count() );
#endif
				}
			}
			m_vars.erase( m_layer );
		}
		--m_layer;
	}
}

size_t vars_t::layer_count() const { return m_layer; }