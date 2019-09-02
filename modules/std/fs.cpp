/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <regex>

#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../src/VM/Core.hpp"

int exec_internal( const std::string & cmd );

enum FSEnt {
	FILES = 1 << 0,
	DIRS = 1 << 1,
	RECURSE = 1 << 2,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Class ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_file_t : public var_base_t
{
	FILE * m_file;
	bool m_copied;
public:
	var_file_t( FILE * file, const int parse_ctr = 0 );
	~var_file_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	bool is_copied() const;
	var_base_t * copy( const int parse_ctr );
	FILE * & get();
};
#define AS_FILE( x ) static_cast< var_file_t * >( x )

var_file_t::var_file_t( FILE * file, const int parse_ctr )
	: var_base_t( VT_CUSTOM, false, parse_ctr ), m_file( file ),
	  m_copied( false ) {}
var_file_t::~var_file_t() { if( m_file != nullptr && !m_copied ) { fclose( m_file ); } }

std::string var_file_t::type_str() const { return "file_t"; }
std::string var_file_t::to_str() const { return "file_t"; }
mpz_class var_file_t::to_int() const { return m_file == nullptr ? 0 : 1; }
bool var_file_t::to_bool() const { return m_file != nullptr; }
var_base_t * var_file_t::copy( const int parse_ctr )
{
	m_copied = true;
	return new var_file_t( m_file, parse_ctr );
}

FILE * & var_file_t::get() { return m_file; }
bool var_file_t::is_copied() const { return m_copied; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * file_open( vm_state_t & vm, func_call_data_t & fcd )
{
	const std::string & file = AS_STR( fcd.args[ 0 ] )->get();
	const std::string & mode = AS_STR( fcd.args[ 1 ] )->get();
	return new var_file_t( fopen( file.c_str(), mode.c_str() ) );
}

var_base_t * file_open_existing( vm_state_t & vm, func_call_data_t & fcd )
{
	var_file_t * file = AS_FILE( fcd.args[ 0 ] );
	const std::string & fname = AS_STR( fcd.args[ 1 ] )->get();
	const std::string & fmode = AS_STR( fcd.args[ 2 ] )->get();
	if( file->get() != NULL && !file->is_copied() ) {
		fclose( file->get() );
	}
	file->get() = fopen( fname.c_str(), fmode.c_str() );
	return TRUE_FALSE( file->get() != NULL );
}

var_base_t * file_close_existing( vm_state_t & vm, func_call_data_t & fcd )
{
	var_file_t * file = AS_FILE( fcd.args[ 0 ] );
	if( file->get() != NULL && !file->is_copied() ) {
		fclose( file->get() );
		file->get() = NULL;
	}
	return nullptr;
}

var_base_t * file_write( vm_state_t & vm, func_call_data_t & fcd )
{
	FILE * & f = AS_FILE( fcd.args[ 0 ] )->get();
	if( f == nullptr ) return vm.vars->get( "false" );
	size_t sz = fcd.args.size();
	for( size_t i = 1; i < sz; ++i ) {
		fprintf( f, "%s", fcd.args[ i ]->to_str().c_str() );
	}
	return vm.vars->get( "true" );
}

var_base_t * file_read( vm_state_t & vm, func_call_data_t & fcd )
{
	FILE * & f = AS_FILE( fcd.args[ 0 ] )->get();
	if( f == nullptr ) return vm.vars->get( "false" );

	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	std::string line_str;

	if( ( read = getline( & line, & len, f ) ) == -1 ) {
		if( line ) free( line );
		return vm.vars->get( "false" );
	}

	line_str = line;
	if( line_str.size() > 0 && line_str.back() == '\n' ) {
		line_str.erase( line_str.end() - 1 );
	}
	free( line );

	AS_STR( fcd.args[ 1 ] )->get() = line_str;

	return vm.vars->get( "true" );
}

var_base_t * file_read_full( vm_state_t & vm, func_call_data_t & fcd )
{
	FILE * & f = AS_FILE( fcd.args[ 0 ] )->get();
	if( f == nullptr ) return vm.vars->get( "false" );

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
		lines.push_back( new var_str_t( line_str ) );
	}

	if( line ) free( line );

	AS_VEC( fcd.args[ 1 ] )->clear();
	AS_VEC( fcd.args[ 1 ] )->get() = lines;

	return vm.vars->get( "true" );
}

var_base_t * is_open( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( AS_FILE( fcd.args[ 0 ] )->get() != NULL );
}

void get_entries_internal( const std::string & dir_str, std::vector< var_base_t * > & v,
			   const size_t & flags, const int parse_ctr, const std::regex & regex )
{
	DIR * dir;
	struct dirent * ent;
	if( ( dir = opendir( dir_str.c_str() ) ) == NULL ) return;

	while( ( ent = readdir( dir ) ) != NULL ) {
		if( strcmp( ent->d_name, "." ) == 0 || strcmp( ent->d_name, ".." ) == 0 ) continue;
		std::string entry = dir_str + ent->d_name;
		if( ( !( flags & FSEnt::RECURSE ) || ent->d_type != DT_DIR ) && !std::regex_match( entry, regex ) ) {
			continue;
		}
		if( ent->d_type == DT_DIR ) {
			if( flags & FSEnt::RECURSE ) {
				get_entries_internal( entry + "/", v, flags, parse_ctr, regex );
			} else if( flags & FSEnt::DIRS ) {
				v.push_back( new var_str_t( entry, parse_ctr ) );
			}
			continue;
		}
		if( flags & FSEnt::FILES || flags & FSEnt::RECURSE ) {
			v.push_back( new var_str_t( entry, parse_ctr ) );
		}
	}
	closedir( dir );
}

var_base_t * get_entries( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > v;
	std::string dir_str = AS_STR( fcd.args[ 1 ] )->get();
	size_t flags = fcd.args.size() <= 2 ? 1 : mpz_to_size_t( AS_INT( fcd.args[ 2 ] )->get() );
	std::string regex_str = fcd.args.size() <= 3 ? "(.*)" : fcd.args[ 3 ]->to_str();
	std::regex regex( regex_str );
	if( dir_str.size() > 0 && dir_str.back() != '/' ) dir_str += "/";
	get_entries_internal( dir_str, v, flags, fcd.parse_ctr, regex );
	return new var_vec_t( v );
}

var_base_t * _exists( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( access( fcd.args[ 1 ]->to_str().c_str(), F_OK ) != -1 );
}

var_base_t * _remove( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( exec_internal( "rm -rf " + fcd.args[ 1 ]->to_str() ) == 0 );
}

REGISTER_MODULE( fs )
{
	vm.funcs.add( { "fopen", 2, 2, { "str", "str" }, FnType::MODULE, { .modfn = file_open }, true } );

	functions_t & ft = vm.typefuncs[ "file_t" ];
	ft.add( { "reopen", 2, 2, { "str", "str" }, FnType::MODULE, { .modfn = file_open_existing }, false } );
	ft.add( { "close", 0, 0, {}, FnType::MODULE, { .modfn = file_close_existing }, false } );
	ft.add( { "write", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = file_write }, false } );
	ft.add( { "read", 1, 1, { "str" }, FnType::MODULE, { .modfn = file_read }, false } );
	ft.add( { "read_all", 1, 1, { "vec" }, FnType::MODULE, { .modfn = file_read_full }, false } );
	ft.add( { "is_open", 0, 0, {}, FnType::MODULE, { .modfn = is_open }, false } );

	functions_t & fst = vm.typefuncs[ "_fs_t" ];
	fst.add( { "dir_entries", 1, 3, { "str", "int", "str" }, FnType::MODULE, { .modfn = get_entries }, true } );
	fst.add( { "exists", 1, 1, { "str" }, FnType::MODULE, { .modfn = _exists }, false } );
	fst.add( { "remove", 1, 1, { "str" }, FnType::MODULE, { .modfn = _remove }, false } );
}

int exec_internal( const std::string & cmd )
{
	FILE * pipe = popen( cmd.c_str(), "r" );
	if( !pipe ) return false;
	char * line = NULL;
	size_t len = 0;
	ssize_t nread;

	while( ( nread = getline( & line, & len, pipe ) ) != -1 );
	free( line );
	int res = pclose( pipe );
	return WEXITSTATUS( res );
}