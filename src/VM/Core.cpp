/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../FE/Env.hpp"
#include "../FE/FS.hpp"

#include "LoadFile.hpp"
#include "Core.hpp"

#ifdef __APPLE__
const char * LIB_EXT = ".dylib";
#else
const char * LIB_EXT = ".so";
#endif

vm_state_t::vm_state_t() : flags( 0 ), exit_called( false ), exit_status( 0 ),
	inc_dirs( new var_vec_t( {}, 0 ) ), lib_dirs( new var_vec_t( {}, 0 ) ),
	none( new var_none_t( 0, 0 ) ), nil( new var_nil_t( 0, 0 ) ),
	vars( new vars_t ), dlib( new dyn_lib_t() ),
	consts( new consts_t() ), stack( new vm_stack_t() )
{
#ifdef BUILD_PREFIX_DIR
	AS_VEC( inc_dirs )->get().push_back( new var_str_t( STRINGIFY( BUILD_PREFIX_DIR ) "/include/ethereal", 0 ) );
	AS_VEC( lib_dirs )->get().push_back( new var_str_t( STRINGIFY( BUILD_PREFIX_DIR ) "/lib/ethereal", 0 ) );
#endif
	vars->add( "__INC_DIRS__", inc_dirs );
	vars->add( "__LIB_DIRS__", lib_dirs );
	vars->add( "nil", nil );
}

vm_state_t::~vm_state_t()
{
	delete stack;
	delete consts;
	delete vars;
	VAR_DREF( none );
	for( auto & struct_ : structs ) delete struct_.second;
	// see: https://gforge.inria.fr/tracker/?func=detail&atid=619&aid=9783&group_id=136
	// clears the log(), pow(), ... cache of mpfr library
	mpfr_free_cache();
	delete dlib;
	for( auto & src : srcs ) delete src.second;
}

bool set_init_mods( vm_state_t & vm )
{
	std::vector< std::string > mods = { "core" };

	for( auto & m : mods ) {
		std::string name = m;
		std::string file = name + ".et";

		if( !mod_exists( file, vm.inc_dirs ) ) {
			fprintf( stderr, "could not find file '%s' for importing\n", name.c_str() );
			fprintf( stderr, "checked the following paths:\n" );
			for( auto & loc : AS_VEC( vm.inc_dirs )->get() ) {
				fprintf( stderr, "-> %s\n", ( loc->to_str() + "/" + file ).c_str() );
			}
			return false;
		}

		int ret = load_src( vm, file );
		if( ret != E_OK ) {
			fprintf( stderr, "could not import '%s', see the error above; aborting\n",
				 file.c_str() );
			return false;
		}
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

bool mod_exists( std::string & file, var_vec_t * locs )
{
	if( file.front() != '~' && file.front() != '/' && file.front() != '.' ) {
		for( auto & _loc : AS_VEC( locs )->get() ) {
			std::string loc = _loc->to_str();
			if( fexists( loc + "/" + file ) ) {
				file = loc + "/" + file;
				return true;
			}
		}
	} else {
		if( file.front() == '~' ) {
			file.erase( file.begin() );
			std::string home = GetEnv( "HOME" );
			file.insert( file.begin(), home.begin(), home.end() );
		}

		return fexists( file );
	}
	return false;
}
