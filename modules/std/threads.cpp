/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <thread>
#include <future>
#include <chrono>
#include <sstream>
#include <sys/wait.h>

#include "../../src/VM/Core.hpp"

int exec_command( const std::string & cmd );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Class ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_thread_t : public var_base_t
{
	std::thread * m_thread;
	std::shared_future< int > m_res;
	bool m_copied;
	int m_id;
public:
	var_thread_t( std::thread * thread, std::shared_future< int > & res, const int id, const int parse_ctr = 0 );
	~var_thread_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	bool is_copied() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	std::thread * & get_thread();
	std::shared_future< int > & get_future();
	int get_id();

};
#define AS_THREAD( x ) static_cast< var_thread_t * >( x )

var_thread_t::var_thread_t( std::thread * thread, std::shared_future< int > & res, const int id,
			    const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, parse_ctr ), m_thread( thread ), m_res( res ),
	  m_id( id ), m_copied( false ) {}
var_thread_t::~var_thread_t() { if( m_thread != nullptr && !m_copied ) { m_thread->join(); delete m_thread; } }

std::string var_thread_t::type_str() const { return "thread_t"; }
std::string var_thread_t::to_str() const
{
	std::ostringstream oss;
	oss << m_thread->get_id();
	return oss.str();
}

mpz_class var_thread_t::to_int() const { return m_res.valid() ? 1 : 0; }
bool var_thread_t::to_bool() const { return m_res.valid(); }
var_base_t * var_thread_t::copy( const int parse_ctr )
{
	m_copied = true;
	return new var_thread_t( m_thread, m_res, m_id, parse_ctr );
}
void var_thread_t::assn( var_base_t * b )
{
	m_thread = AS_THREAD( b )->m_thread;
	AS_THREAD( b )->m_copied = true;
	m_res = AS_THREAD( b )->m_res;
}

std::thread * & var_thread_t::get_thread() { return m_thread; }
std::shared_future< int > & var_thread_t::get_future() { return m_res; }
int var_thread_t::get_id() { return m_id; }
bool var_thread_t::is_copied() const { return m_copied; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * threads_get_nproc( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( ( size_t )std::thread::hardware_concurrency() );
}

var_base_t * thread_new_exec( vm_state_t & vm, func_call_data_t & fcd )
{
	std::packaged_task< int( const std::string & ) > task( exec_command );
	std::shared_future< int > fut( task.get_future() );
	int id = fcd.args.size() > 2 ? fcd.args[ 2 ]->to_int().get_si() : -1;
	return new var_thread_t( new std::thread( std::move( task ), fcd.args[ 1 ]->to_str() ), fut, id );
}

var_base_t * thread_get_id( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( AS_THREAD( fcd.args[ 0 ] )->get_id() );
}

var_base_t * thread_is_done( vm_state_t & vm, func_call_data_t & fcd )
{
	std::shared_future< int > & fut = AS_THREAD( fcd.args[ 0 ] )->get_future();
	// TODO: properly fix fut.wait_for() function to avoid crashes
	return TRUE_FALSE( fut.valid() && fut.wait_for( std::chrono::milliseconds( 10 ) ) == std::future_status::ready );
}

var_base_t * thread_get_res( vm_state_t & vm, func_call_data_t & fcd )
{
	std::shared_future< int > & fut = AS_THREAD( fcd.args[ 0 ] )->get_future();
	// TODO: properly fix fut.wait_for() function to avoid crashes
	if( !fut.valid() || fut.wait_for( std::chrono::milliseconds( 10 ) ) != std::future_status::ready ) return vm.nil;
	return new var_int_t( fut.get() );
}

REGISTER_MODULE( threads )
{
	vm.funcs.add( { "threads_get_nproc", 0, 0, {}, FnType::MODULE, { .modfn = threads_get_nproc }, true } );

	functions_t & threadsfns = vm.typefuncs[ "_threads_t" ];
	threadsfns.add( { "new_exec", 1, 2, { "str", "int" }, FnType::MODULE, { .modfn = thread_new_exec }, true } );

	functions_t & threadfns = vm.typefuncs[ "thread_t" ];
	threadfns.add( { "id", 0, 0, {}, FnType::MODULE, { .modfn = thread_get_id }, true } );
	threadfns.add( { "done", 0, 0, {}, FnType::MODULE, { .modfn = thread_is_done }, false } );
	threadfns.add( { "res", 0, 0, {}, FnType::MODULE, { .modfn = thread_get_res }, true } );
}

int exec_command( const std::string & cmd )
{
	FILE * pipe = popen( cmd.c_str(), "r" );
	if( !pipe ) return false;
	char * line = NULL;
	size_t len = 0;
	ssize_t nread;

	while( ( nread = getline( & line, & len, pipe ) ) != -1 ) {
		fprintf( stdout, "%s", line );
	}

	free( line );
	int res = pclose( pipe );
	return WEXITSTATUS( res );
}
