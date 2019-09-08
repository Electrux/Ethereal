/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../FE/FS.hpp"

#include "Core.hpp"

// bcodectr reserved by default because it can't go to more depth
// if it does, it will cause reallocation and invalidate the reference (i) used in exec_internal()
vm_state_t::vm_state_t() : flags( 0 ), exit_called( false ), exit_status( 0 ),
	none( new var_none_t( 0 ) ), nil( new var_nil_t( 0 ) ),
	vars( new vars_t ), dlib( new dyn_lib_t() ),
	consts( new consts_t() ), stack( new vm_stack_t() )
{ bcodectr.reserve( MAX_BCODE_CTR_SRCS ); }

vm_state_t::~vm_state_t()
{
	delete stack;
	delete consts;
	delete vars;
	VAR_DREF( nil );
	VAR_DREF( none );
	for( auto & struct_ : structs ) delete struct_.second;
	delete dlib;
	for( auto & src : srcs ) delete src.second;
}

bool set_init_mods( vm_state_t & vm )
{
	std::vector< std::string > mods = { "core" };

	for( auto & m : mods ) {
		std::string module_name = m + ".so";
		if( m.front() != '~' && m.front() != '/' && m.front() != '.' ) {
			module_name = STRINGIFY( BUILD_PREFIX_DIR ) "/lib/ethereal/pre/lib" + m + ".so";
		}
		if( !fexists( module_name ) ) {
			fprintf( stderr, "failed to find the prelude module library: %s\n", module_name.c_str() );
			return false;
		}
		if( vm.dlib->load( module_name ) == nullptr ) return false;
		init_fnptr_t init_fn = ( init_fnptr_t ) vm.dlib->get( module_name, "init_" + m );
		if( init_fn == nullptr ) {
			fprintf( stderr, "failed to find init function in module library: %s\n", module_name.c_str() );
			return false;
		}
		init_fn( vm );
	}
	return true;
}

size_t mpz_to_size_t( const mpz_class & n )
{
	size_t data;
	mpz_export( & data, 0, -1, sizeof( data ), 0, 0, n.get_mpz_t() );
	return data;
}

std::vector< std::string > str_delimit( const std::string & str, const char ch, const bool first_only )
{
	std::string temp;
	std::vector< std::string > vec;
	bool done = false;

	for( auto c : str ) {
		if( c == ch && ( !first_only || !done ) ) {
			done = true;
			if( temp.empty() ) continue;
			vec.push_back( temp );
			temp.clear();
			continue;
		}

		temp += c;
	}

	if( !temp.empty() ) vec.push_back( temp );
	return vec;
}