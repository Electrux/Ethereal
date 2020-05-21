/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../src/VM/Core.hpp"

var_base_t * make_tuple( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > v;
	for( auto & x : fcd.args ) {
		v.push_back( x->copy( fcd.src_idx, fcd.parse_ctr ) );
	}
	return new var_tuple_t( v );
}

var_base_t * eqt( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_TUPLE( fcd.args[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_TUPLE( fcd.args[ 0 ] )->get();
	if( a.size() != b.size() ) return vm.vars->get( "false" );
	for( auto i1 = a.begin(), i2 = b.begin(); i1 != a.end() && i2 != b.end(); ++i1, ++i2 ) {
		if( ( * i1 )->to_str() != ( * i2 )->to_str() ) return vm.vars->get( "false" );
	}
	return vm.vars->get( "true" );
}

var_base_t * net( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_TUPLE( fcd.args[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_TUPLE( fcd.args[ 0 ] )->get();
	if( a.size() != b.size() ) return vm.vars->get( "true" );
	for( auto i1 = a.begin(), i2 = b.begin(); i1 != a.end() && i2 != b.end(); ++i1, ++i2 ) {
		if( ( * i1 )->to_str() != ( * i2 )->to_str() ) return vm.vars->get( "true" );
	}
	return vm.vars->get( "false" );
}

REGISTER_MODULE( tuple )
{
	vm.funcs.add( { "make_tuple", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = make_tuple }, true } );

	vm.funcs.add( { "==", 2, 2, { "tuple", "tuple" }, FnType::MODULE, { .modfn = eqt }, false } );
	vm.funcs.add( { "!=", 2, 2, { "tuple", "tuple" }, FnType::MODULE, { .modfn = net }, false } );
}
