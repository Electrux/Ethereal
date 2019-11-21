/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <chrono>

#include "../../src/VM/Core.hpp"

typedef std::chrono::high_resolution_clock::time_point time_point_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_time_point_t : public var_base_t
{
	time_point_t m_time;
public:
	var_time_point_t( const time_point_t & time, const int src_idx = 0, const int parse_ctr = 0 );
	~var_time_point_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	time_point_t & get();
};
#define AS_TIME_POINT( x ) static_cast< var_time_point_t * >( x )

var_time_point_t::var_time_point_t( const time_point_t & time, const int src_idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, src_idx, parse_ctr ), m_time( time ) {}
var_time_point_t::~var_time_point_t() {}

std::string var_time_point_t::type_str() const { return "time_point_t"; }
std::string var_time_point_t::to_str() const
{
	return std::to_string( std::chrono::duration_cast< std::chrono::seconds >( m_time.time_since_epoch() ).count() );
}
mpz_class var_time_point_t::to_int() const { return ( size_t )std::chrono::duration_cast< std::chrono::seconds >( m_time.time_since_epoch() ).count(); }
bool var_time_point_t::to_bool() const { return true; }
var_base_t * var_time_point_t::copy( const int src_idx, const int parse_ctr )
{
	return new var_time_point_t( m_time, src_idx, parse_ctr );
}
void var_time_point_t::assn( var_base_t * b )
{
	m_time = AS_TIME_POINT( b )->get();
}
time_point_t & var_time_point_t::get() { return m_time; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * time_now( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_time_point_t( std::chrono::high_resolution_clock::now() );
}

var_base_t * time_to_hrs( vm_state_t & vm, func_call_data_t & fcd )
{
	time_point_t & time = AS_TIME_POINT( fcd.args[ 0 ] )->get();
	return new var_int_t( ( size_t )std::chrono::duration_cast< std::chrono::hours >( time.time_since_epoch() ).count() );
}

var_base_t * time_to_mins( vm_state_t & vm, func_call_data_t & fcd )
{
	time_point_t & time = AS_TIME_POINT( fcd.args[ 0 ] )->get();
	return new var_int_t( ( size_t )std::chrono::duration_cast< std::chrono::minutes >( time.time_since_epoch() ).count() );
}

var_base_t * time_to_secs( vm_state_t & vm, func_call_data_t & fcd )
{
	time_point_t & time = AS_TIME_POINT( fcd.args[ 0 ] )->get();
	return new var_int_t( ( size_t )std::chrono::duration_cast< std::chrono::seconds >( time.time_since_epoch() ).count() );
}

var_base_t * time_to_msecs( vm_state_t & vm, func_call_data_t & fcd )
{
	time_point_t & time = AS_TIME_POINT( fcd.args[ 0 ] )->get();
	return new var_int_t( ( size_t )std::chrono::duration_cast< std::chrono::milliseconds >( time.time_since_epoch() ).count() );
}

var_base_t * time_to_usecs( vm_state_t & vm, func_call_data_t & fcd )
{
	time_point_t & time = AS_TIME_POINT( fcd.args[ 0 ] )->get();
	return new var_int_t( ( size_t )std::chrono::duration_cast< std::chrono::microseconds >( time.time_since_epoch() ).count() );
}

var_base_t * time_diff( vm_state_t & vm, func_call_data_t & fcd )
{
	time_point_t & time1 = AS_TIME_POINT( fcd.args[ 1 ] )->get();
	time_point_t & time2 = AS_TIME_POINT( fcd.args[ 0 ] )->get();
	return new var_time_point_t( time_point_t( time1 - time2 ) );
}

REGISTER_MODULE( time )
{
	functions_t & timemodfns = vm.typefuncs[ "_time_t" ];
	timemodfns.add( { "now", 0,  0, {}, FnType::MODULE, { .modfn = time_now }, true } );

	functions_t & timefns = vm.typefuncs[ "time_point_t" ];
	timefns.add( { "hours", 0,  0, {}, FnType::MODULE, { .modfn = time_to_hrs }, true } );
	timefns.add( { "mins",  0,  0, {}, FnType::MODULE, { .modfn = time_to_mins }, true } );
	timefns.add( { "secs",  0,  0, {}, FnType::MODULE, { .modfn = time_to_secs }, true } );
	timefns.add( { "msecs", 0,  0, {}, FnType::MODULE, { .modfn = time_to_msecs }, true } );
	timefns.add( { "usecs", 0,  0, {}, FnType::MODULE, { .modfn = time_to_usecs }, true } );

	vm.funcs.add( { "-", 2,  2, { "time_point_t", "time_point_t" }, FnType::MODULE, { .modfn = time_diff }, true } );
}