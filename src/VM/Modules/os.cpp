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

#include "../Vars/Base.hpp"
#include "../Core.hpp"

var_base_t * sleep_custom( std::vector< var_base_t * > & vars )
{
	std::this_thread::sleep_for(
		std::chrono::milliseconds( vars[ 1 ]->to_int().get_ui() )
	);
	return nullptr;
}

var_base_t * get_env( std::vector< var_base_t * > & vars )
{
	std::string var = vars[ 1 ]->to_str();
	const char * env = getenv( var.c_str() );
	return new var_str_t( env == NULL ? "" : env, vars[ 0 ]->parse_ctr() );
}

var_base_t * set_env( std::vector< var_base_t * > & vars )
{
	std::string var = vars[ 1 ]->to_str();
	std::string val = vars[ 2 ]->to_str();

	bool overwrite = false;
	if( vars.size() > 3 ) overwrite = vars[ 3 ]->to_bool();
	return new var_int_t( setenv( var.c_str(), val.c_str(), overwrite ), vars[ 0 ]->parse_ctr() );
}

var_base_t * exec_custom( std::vector< var_base_t * > & vars )
{
	std::string cmd = vars[ 1 ]->to_str();

	FILE * pipe = popen( cmd.c_str(), "r" );
	if( !pipe ) return new var_int_t( 1, vars[ 0 ]->parse_ctr() );
	char * line = NULL;
	size_t len = 0;
	ssize_t nread;

	while( ( nread = getline( & line, & len, pipe ) ) != -1 ) {
		fprintf( stdout, "%s", line );
	}
	free( line );
	int res = pclose( pipe );

	res = WEXITSTATUS( res );
	return new var_int_t( res, vars[ 0 ]->parse_ctr() );
}

REGISTER_MODULE( os )
{
	functions_t & os = vm.typefuncs[ "_os_t" ];

	os.add( { "sleep", 1, 1, { "int" }, FnType::MODULE, { .modfn = sleep_custom }, false } );
	os.add( { "get_env", 1, 1, { "str" }, FnType::MODULE, { .modfn = get_env }, true } );
	os.add( { "set_env", 2, 3, { "str", "str", "_any_" }, FnType::MODULE, { .modfn = set_env }, true } );
	os.add( { "exec", 1, 1, { "str" }, FnType::MODULE, { .modfn = exec_custom }, true } );
}