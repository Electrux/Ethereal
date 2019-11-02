/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <chrono>
#include <thread>
#include <cstdlib>
#include <sys/wait.h>

#include "../../src/VM/Core.hpp"

int exec_internal( const std::string & file );
std::string dir_part( const std::string & full_loc );

var_base_t * sleep_custom( vm_state_t & vm, func_call_data_t & fcd )
{
	std::this_thread::sleep_for(
		std::chrono::milliseconds( fcd.args[ 1 ]->to_int().get_ui() )
	);
	return nullptr;
}

var_base_t * get_env( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string var = fcd.args[ 1 ]->to_str();
	const char * env = getenv( var.c_str() );
	return new var_str_t( env == NULL ? "" : env );
}

var_base_t * set_env( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string var = fcd.args[ 1 ]->to_str();
	std::string val = fcd.args[ 2 ]->to_str();

	bool overwrite = false;
	if( fcd.args.size() > 3 ) overwrite = fcd.args[ 3 ]->to_bool();
	return new var_int_t( setenv( var.c_str(), val.c_str(), overwrite ) );
}

var_base_t * exec_custom( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string cmd = fcd.args[ 1 ]->to_str();

	FILE * pipe = popen( cmd.c_str(), "r" );
	if( !pipe ) return new var_int_t( 1 );
	char * line = NULL;
	size_t len = 0;
	ssize_t nread;

	while( ( nread = getline( & line, & len, pipe ) ) != -1 ) {
		fprintf( stdout, "%s", line );
	}
	free( line );
	int res = pclose( pipe );

	res = WEXITSTATUS( res );
	return new var_int_t( res );
}

var_base_t * install( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string src = AS_STR( fcd.args[ 1 ] )->get(),
		    dest = AS_STR( fcd.args[ 2 ] )->get();

	if( src.empty() || dest.empty() ) {
		return new var_int_t( 0 );
	}

	if( exec_internal( "mkdir -p " + dest ) != 0 ) {
		return new var_int_t( -1 );
	}
	std::string cmd_str;
#if __linux__ || __ANDROID__
	cmd_str = "cp -r --remove-destination " + src + " " + dest;
#elif __APPLE__ || __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__
	cmd_str = "cp -rf " + src + " " + dest;
#endif
	return new var_int_t( exec_internal( cmd_str ) );
}

var_base_t * os_get_name( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string os_str;
#if __ANDROID__
	os_str = "android";
#elif __linux__
	os_str = "linux";
#elif __APPLE__
	os_str = "macos";
#elif __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__
	os_str = "bsd";
#endif
	return new var_str_t( os_str, 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Extra Functions ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * os_mkdir( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string dest = fcd.args[ 1 ]->to_str();

	for( size_t i = 2; i < fcd.args_count; ++i ) {
		std::string tmpdest = fcd.args[ i ]->to_str();
		if( tmpdest.empty() ) continue;
		dest += " " + tmpdest;
	}
	return new var_int_t( exec_internal( "mkdir -p " + dest ) );
}

var_base_t * os_rm( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string dest = fcd.args[ 1 ]->to_str();

	for( size_t i = 2; i < fcd.args_count; ++i ) {
		std::string tmpdest = fcd.args[ i ]->to_str();
		if( tmpdest.empty() ) continue;
		dest += " " + tmpdest;
	}
	if( dest.empty() ) {
		return new var_int_t( 0 );
	}
	return new var_int_t( exec_internal( "rm -r " + dest ) );
}

REGISTER_MODULE( os )
{
	functions_t & os = vm.typefuncs[ "_os_t" ];

	os.add( { "sleep", 1, 1, { "int" }, FnType::MODULE, { .modfn = sleep_custom }, false } );
	os.add( { "get_env", 1, 1, { "str" }, FnType::MODULE, { .modfn = get_env }, true } );
	os.add( { "set_env", 2, 3, { "str", "str", "_any_" }, FnType::MODULE, { .modfn = set_env }, true } );
	os.add( { "exec", 1, 1, { "str" }, FnType::MODULE, { .modfn = exec_custom }, true } );
	os.add( { "install", 2, 2, { "str", "str" }, FnType::MODULE, { .modfn = install }, true } );

	vm.funcs.add( { "os_get_name", 0, 0, {}, FnType::MODULE, { .modfn = os_get_name }, true } );

	os.add( { "mkdir", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = os_mkdir }, true } );
	os.add( { "rm", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = os_rm }, true } );
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

std::string dir_part( const std::string & full_loc )
{
	auto loc = full_loc.find_last_of( '/' );
	if( loc == std::string::npos ) return ".";
	if( loc == 0 ) return "/";
	return full_loc.substr( 0, loc );
}