/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../Core.hpp"

var_base_t * insert( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_map< std::string, var_base_t * > & map = AS_MAP( fcd.args[ 0 ] )->get();
	std::string key = fcd.args[ 1 ]->to_str();
	if( map.find( key ) != map.end() ) {
		VAR_DREF( map[ key ] );
	}
	map[ key ] = fcd.args[ 2 ]->copy( fcd.args[ 0 ]->parse_ctr() );
	return nullptr;
}

var_base_t * _delete( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_map< std::string, var_base_t * > & map = AS_MAP( fcd.args[ 0 ] )->get();
	std::string key = fcd.args[ 1 ]->to_str();
	if( map.find( key ) != map.end() ) {
		VAR_DREF( map[ key ] );
		map.erase( key );
		return vm.vars->get( "true" );
	}
	return vm.vars->get( "false" );
}

var_base_t * len( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( ( int )AS_MAP( fcd.args[ 0 ] )->get().size(), fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * clear( vm_state_t & vm, func_call_data_t & fcd )
{
	AS_MAP( fcd.args[ 0 ] )->clear();
	return nullptr;
}

var_base_t * find( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_map< std::string, var_base_t * > & map = AS_MAP( fcd.args[ 0 ] )->get();
	std::string key = fcd.args[ 1 ]->to_str();
	return TRUE_FALSE( map.find( key ) != map.end() );
}

REGISTER_MODULE( map )
{
	functions_t & mapfns = vm.typefuncs[ "map" ];

	mapfns.add( { "insert", 2, 2, { "str", "_any_" }, FnType::MODULE, { .modfn = insert }, false } );
	mapfns.add( { "delete", 1, 1, { "str" }, FnType::MODULE, { .modfn = _delete }, false } );
	mapfns.add( { "len", 0, 0, {}, FnType::MODULE, { .modfn = len }, true } );
	mapfns.add( { "clear", 0, 0, {}, FnType::MODULE, { .modfn = clear }, false } );
	mapfns.add( { "find", 1, 1, { "str" }, FnType::MODULE, { .modfn = find }, false } );
}
