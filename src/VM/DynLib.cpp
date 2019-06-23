/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdio>
#include <string>
#include <unordered_map>
#include <dlfcn.h>

#include "DynLib.hpp"

dyn_lib_t::dyn_lib_t() {}

dyn_lib_t::~dyn_lib_t()
{
	for( auto & e : m_handles ) {
		if( e.second != nullptr ) dlclose( e.second );
	}
}

void * dyn_lib_t::load( const std::string & file )
{
	if( m_handles.find( file ) == m_handles.end() ) {
		void * tmp = dlopen( file.c_str(), RTLD_LAZY );
		if( tmp == nullptr ) {
			fprintf( stderr, "dyn_lib_t error: %s\n", dlerror() );
			return nullptr;
		}
		m_handles[ file ] = tmp;
	}
	return m_handles[ file ];
}

void dyn_lib_t::unload( const std::string & file )
{
	if( m_handles.find( file ) == m_handles.end() ) return;
	dlclose( m_handles[ file ] );
	m_handles.erase( file );
}

void * dyn_lib_t::get_sym( const std::string & file, const std::string & sym )
{
	if( m_handles.find( file ) == m_handles.end() ) return nullptr;
	return dlsym( m_handles[ file ], sym.c_str() );
}
