/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdio>

#include "../Vars/Base.hpp"
#include "../Core.hpp"

var_base_t * file_open( std::vector< var_base_t * > & vars )
{
	const std::string & file = AS_STR( vars[ 0 ] )->get();
	const std::string & mode = AS_STR( vars[ 1 ] )->get();
	FILE * f = fopen( file.c_str(), mode.c_str() );
	std::unordered_map< std::string, var_base_t * > smap;
	var_int_t * err = new var_int_t( 0, vars[ 0 ]->parse_ctr() );
	var_int_t * dat = new var_int_t( 0, vars[ 0 ]->parse_ctr() );
	smap[ "err" ] = err;
	smap[ "handle" ] = dat;
	var_struct_t * res = new var_struct_t( "file_t", smap, vars[ 0 ]->parse_ctr() );
	if( f == nullptr ) {
		err->get() = -1;
		return res;
	}
	dat->get() = std::to_string( ( unsigned long long )f );
	return res;
}

var_base_t * file_close( std::vector< var_base_t * > & vars )
{
	mpz_class & err = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get();
	mpz_class & dat = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get();
	if( dat == 0 ) return nullptr;
	FILE * f = ( FILE * )std::stoull( dat.get_str() );
	dat = 0;
	err = fclose( f );
	return nullptr;
}

var_base_t * file_write( std::vector< var_base_t * > & vars )
{
	if( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get() < 0 ) return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	unsigned long long file = std::stoull( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get().get_str() );
	if( file == ( unsigned long long )-1 ) return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	FILE * f = ( FILE * )file;
	size_t sz = vars.size();
	for( size_t i = 1; i < sz; ++i ) {
		fprintf( f, "%s", vars[ i ]->to_str().c_str() );
	}
	return new var_bool_t( true, vars[ 0 ]->parse_ctr() );
}

var_base_t * file_read( std::vector< var_base_t * > & vars )
{
	if( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get() < 0 ) return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	unsigned long long file = std::stoull( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get().get_str() );
	if( file == ( unsigned long long )-1 ) return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	FILE * f = ( FILE * )file;

	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	std::string line_str;

	if( ( read = getline( & line, & len, f ) ) == -1 ) {
		if( line ) free( line );
		return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	}

	line_str = line;
	if( line_str.size() > 0 && line_str.back() == '\n' ) {
		line_str.erase( line_str.end() - 1 );
	}
	if( line ) free( line );

	AS_STR( vars[ 1 ] )->get() = line_str;

	return new var_bool_t( true, vars[ 0 ]->parse_ctr() );
}

var_base_t * file_read_full( std::vector< var_base_t * > & vars )
{
	if( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get() < 0 ) return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	unsigned long long file = std::stoull( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get().get_str() );
	if( file == ( unsigned long long )-1 ) return new var_bool_t( false, vars[ 0 ]->parse_ctr() );
	FILE * f = ( FILE * )file;

	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	std::vector< var_base_t * > lines;
	std::string line_str;

	while( ( read = getline( & line, & len, f ) ) != -1 ) {
		line_str = line;
		if( line_str.size() > 0 && line_str.back() == '\n' ) {
			line_str.erase( line_str.end() - 1 );
		}
		lines.push_back( new var_str_t( line_str, vars[ 0 ]->parse_ctr() ) );
	}

	if( line ) free( line );

	AS_VEC( vars[ 1 ] )->clear();
	AS_VEC( vars[ 1 ] )->get() = lines;

	return new var_bool_t( true, vars[ 0 ]->parse_ctr() );
}

var_base_t * is_open( std::vector< var_base_t * > & vars )
{
	return new var_bool_t( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get() == 0 &&
			       AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "handle" ] )->get() > 0, vars[ 0 ]->parse_ctr() );
}

var_base_t * is_good( std::vector< var_base_t * > & vars )
{
	return new var_bool_t( AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get() == 0, vars[ 0 ]->parse_ctr() );
}

var_base_t * get_err( std::vector< var_base_t * > & vars )
{
	return AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ];
}

var_base_t * err_str( std::vector< var_base_t * > & vars )
{
	mpz_class & err = AS_INT( AS_STRUCT( vars[ 0 ] )->get_val()[ "err" ] )->get();
	var_str_t * res = new var_str_t( "ok", vars[ 0 ]->parse_ctr() );
	if( err == 0 ) return res;
	else if( err == -1 ) res->get() = "could not open given file";
	return res;
}

REGISTER_MODULE( fs )
{
	vm.funcs.add( { "fopen", 2, 2, { "str", "str" }, FnType::MODULE, { .modfn = file_open }, true } );
	vm.funcs.add( { "fclose", 1, 1, { "file_t" }, FnType::MODULE, { .modfn = file_close }, false } );

	functions_t & ft = vm.typefuncs[ "file_t" ];
	ft.add( { "write", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = file_write }, true } );
	ft.add( { "is_open", 0, 0, {}, FnType::MODULE, { .modfn = is_open }, true } );
	ft.add( { "is_good", 0, 0, {}, FnType::MODULE, { .modfn = is_good }, true } );
	ft.add( { "get_err", 0, 0, {}, FnType::MODULE, { .modfn = get_err }, false } );
	ft.add( { "err_str", 0, 0, {}, FnType::MODULE, { .modfn = err_str }, true } );
	ft.add( { "read", 1, 1, { "str" }, FnType::MODULE, { .modfn = file_read }, true } );
	ft.add( { "read_all", 1, 1, { "vec" }, FnType::MODULE, { .modfn = file_read_full }, true } );
}