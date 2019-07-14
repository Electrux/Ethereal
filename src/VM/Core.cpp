/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Core.hpp"

vm_state_t::vm_state_t() : stack( new vm_stack_t() ) {}

vm_state_t::~vm_state_t()
{
	delete stack;
	for( auto & src : srcmap ) {
		delete src.second;
	}
}

bool set_init_mods( vm_state_t & vm )
{
	std::vector< std::string > mods = { "core" };

	for( auto & m : mods ) {
		std::string module_name = m + ".so";
		if( m.front() != '~' && m.front() != '/' && m.front() != '.' ) {
			module_name = STRINGIFY( BUILD_PREFIX_DIR ) "/lib/ethereal/lib" + m + ".so";
		}
		if( !fexists( module_name ) ) {
			fprintf( stderr, "failed to find the prelude module library: %s\n", module_name.c_str() );
			return false;
		}
		if( vm.dlib.load( module_name ) == nullptr ) return false;
		init_fnptr_t init_fn = ( init_fnptr_t ) vm.dlib.get( module_name, "init_" + m );
		if( init_fn == nullptr ) {
			fprintf( stderr, "failed to find init function in module library: %s\n", module_name.c_str() );
			return false;
		}
		init_fn( vm );
	}
	return true;
}