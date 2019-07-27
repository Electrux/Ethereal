/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <unordered_set>

#include "../Vars/Base.hpp"
#include "../Core.hpp"

var_base_t * set_create( std::vector< var_base_t * > & vars )
{
	std::unordered_set< std::string > * s = new std::unordered_set< std::string >();
	std::unordered_map< std::string, var_base_t * > smap;
	var_int_t * dat = new var_int_t( 0, 0 );
	smap[ "handle" ] = dat;
	var_struct_t * res = new var_struct_t( "_set_t", smap, 0 );
	if( s == nullptr ) {
		return res;
	}
	dat->get() = std::to_string( ( unsigned long long )s );
	return res;
}

var_base_t * set_destroy( std::vector< var_base_t * > & vars )
{
	mpz_class & dat = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get();
	if( dat == 0 ) return nullptr;
	std::unordered_set< std::string > * s = ( std::unordered_set< std::string > * )std::stoull( dat.get_str() );
	delete s;
	dat = 0;
	return nullptr;
}

var_base_t * set_insert( std::vector< var_base_t * > & vars )
{
	mpz_class & dat = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get();
	if( dat == 0 ) return nullptr;
	std::unordered_set< std::string > * s = ( std::unordered_set< std::string > * )std::stoull( dat.get_str() );
	for( size_t i = 1; i < vars.size(); ++i ) {
		s->insert( vars[ i ]->to_str() );
	}
	return nullptr;
}

var_base_t * set_erase( std::vector< var_base_t * > & vars )
{
	mpz_class & dat = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get();
	if( dat == 0 ) return nullptr;
	std::unordered_set< std::string > * s = ( std::unordered_set< std::string > * )std::stoull( dat.get_str() );
	for( size_t i = 1; i < vars.size(); ++i ) {
		s->erase( vars[ i ]->to_str() );
	}
	return nullptr;
}

var_base_t * set_contains( std::vector< var_base_t * > & vars )
{
	mpz_class & dat = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get();
	if( dat == 0 ) return nullptr;
	std::unordered_set< std::string > * s = ( std::unordered_set< std::string > * )std::stoull( dat.get_str() );
	return new var_bool_t( s->find( vars[ 1 ]->to_str() ) != s->end(), vars[ 0 ]->parse_ctr() );
}

REGISTER_MODULE( set )
{
	vm.funcs.add( { "set_create", 0, 0, {}, FnType::MODULE, { .modfn = set_create }, true } );
	vm.funcs.add( { "set_destroy", 1, 1, { "_set_t" }, FnType::MODULE, { .modfn = set_destroy }, false } );

	functions_t & st = vm.typefuncs[ "_set_t" ];
	st.add( { "insert", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = set_insert }, false } );
	st.add( { "erase", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = set_erase }, false } );
	st.add( { "contains", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = set_contains }, true } );
}