/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdio>
#include <dirent.h>

#include "../Core.hpp"

enum FSEnt {
	FILES = 1 << 0,
	DIRS = 1 << 1,
	RECURSE = 1 << 2,
};

class var_file_t : public var_base_t
{
	FILE * m_file;
	bool m_copied;
public:
	var_file_t( FILE * file, const int parse_ctr );
	~var_file_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	FILE * & get();
};
#define AS_FILE( x ) static_cast< var_file_t * >( x )

var_file_t::var_file_t( FILE * file, const int parse_ctr )
	: var_base_t( VT_CUSTOM, parse_ctr, false ), m_file( file ),
	  m_copied( false ) {}
var_file_t::~var_file_t() { if( m_file != nullptr && !m_copied ) { fclose( m_file ); } }

std::string var_file_t::type_str() const { return "_file_t"; }
std::string var_file_t::to_str() const { return "_file_t"; }
mpz_class var_file_t::to_int() const { return m_file == nullptr ? 0 : 1; }
bool var_file_t::to_bool() const { return m_file != nullptr; }
var_base_t * var_file_t::copy( const int parse_ctr )
{
	m_copied = true;
	return new var_file_t( m_file, parse_ctr );
}

FILE * & var_file_t::get() { return m_file; }

var_base_t * file_open( vm_state_t & vm, func_call_data_t & fcd )
{
	const std::string & file = AS_STR( fcd.args[ 0 ] )->get();
	const std::string & mode = AS_STR( fcd.args[ 1 ] )->get();
	return new var_file_t(
		fopen( file.c_str(), mode.c_str() ),
		fcd.args[ 0 ]->parse_ctr()
	);
}

var_base_t * file_write( vm_state_t & vm, func_call_data_t & fcd )
{
	FILE * & f = AS_FILE( fcd.args[ 0 ] )->get();
	if( f == nullptr ) return new var_bool_t( false, fcd.args[ 0 ]->parse_ctr() );
	size_t sz = fcd.args.size();
	for( size_t i = 1; i < sz; ++i ) {
		fprintf( f, "%s", fcd.args[ i ]->to_str().c_str() );
	}
	return new var_bool_t( true, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * file_read( vm_state_t & vm, func_call_data_t & fcd )
{
	FILE * & f = AS_FILE( fcd.args[ 0 ] )->get();
	if( f == nullptr ) return new var_bool_t( false, fcd.args[ 0 ]->parse_ctr() );

	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	std::string line_str;

	if( ( read = getline( & line, & len, f ) ) == -1 ) {
		if( line ) free( line );
		return new var_bool_t( false, fcd.args[ 0 ]->parse_ctr() );
	}

	line_str = line;
	if( line_str.size() > 0 && line_str.back() == '\n' ) {
		line_str.erase( line_str.end() - 1 );
	}
	if( line ) free( line );

	AS_STR( fcd.args[ 1 ] )->get() = line_str;

	return new var_bool_t( true, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * file_read_full( vm_state_t & vm, func_call_data_t & fcd )
{
	FILE * & f = AS_FILE( fcd.args[ 0 ] )->get();
	if( f == nullptr ) return new var_bool_t( false, fcd.args[ 0 ]->parse_ctr() );

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
		lines.push_back( new var_str_t( line_str, fcd.args[ 0 ]->parse_ctr() ) );
	}

	if( line ) free( line );

	AS_VEC( fcd.args[ 1 ] )->clear();
	AS_VEC( fcd.args[ 1 ] )->get() = lines;

	return new var_bool_t( true, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * is_open( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_bool_t( AS_FILE( fcd.args[ 0 ] )->get() != NULL, fcd.args[ 0 ]->parse_ctr() );
}

void get_entries_internal( const std::string & dir_str, std::vector< var_base_t * > & v, const size_t & flags, const int parse_ctr )
{
	DIR * dir;
	struct dirent * ent;
	if( ( dir = opendir( dir_str.c_str() ) ) == NULL ) return;

	while( ( ent = readdir( dir ) ) != NULL ) {
		if( strcmp( ent->d_name, "." ) == 0 || strcmp( ent->d_name, ".." ) == 0 ) continue;
		if( ent->d_type == DT_DIR ) {
			if( flags & FSEnt::RECURSE ) {
				get_entries_internal( dir_str + ent->d_name + "/", v, flags, parse_ctr );
			} else if( flags & FSEnt::DIRS ) {
				v.push_back( new var_str_t( dir_str + ent->d_name, parse_ctr ) );
			}
			continue;
		}
		if( flags & FSEnt::FILES || flags & FSEnt::RECURSE ) {
			v.push_back( new var_str_t( dir_str + ent->d_name, parse_ctr ) );
		}
	}
	closedir( dir );
}

var_base_t * get_entries( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > v;
	std::string dir_str = AS_STR( fcd.args[ 1 ] )->get();
	size_t flags = fcd.args.size() <= 2 ? 1 : mpz_to_size_t( AS_INT( fcd.args[ 2 ] )->get() );
	if( dir_str.size() > 0 && dir_str.back() != '/' ) dir_str += "/";
	get_entries_internal( dir_str, v, flags, fcd.args[ 1 ]->parse_ctr() );
	return new var_vec_t( v, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * _fexists( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_bool_t( access( fcd.args[ 1 ]->to_str().c_str(), F_OK ) != -1, fcd.args[ 1 ]->parse_ctr() );
}

REGISTER_MODULE( fs )
{
	vm.funcs.add( { "fopen", 2, 2, { "str", "str" }, FnType::MODULE, { .modfn = file_open }, true } );

	functions_t & ft = vm.typefuncs[ "_file_t" ];
	ft.add( { "write", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = file_write }, true } );
	ft.add( { "read", 1, 1, { "str" }, FnType::MODULE, { .modfn = file_read }, true } );
	ft.add( { "read_all", 1, 1, { "vec" }, FnType::MODULE, { .modfn = file_read_full }, true } );
	ft.add( { "is_open", 0, 0, {}, FnType::MODULE, { .modfn = is_open }, true } );

	functions_t & fst = vm.typefuncs[ "_fs_t" ];
	fst.add( { "dir_entries", 1, 2, { "str", "int" }, FnType::MODULE, { .modfn = get_entries }, true } );
	fst.add( { "fexists", 1, 1, { "str" }, FnType::MODULE, { .modfn = _fexists }, true } );
}