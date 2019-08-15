/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../Core.hpp"

var_base_t * append( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 0 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 1 ] )->get();
	for( auto & x : b ) {
		a.push_back( x->copy( fcd.args[ 0 ]->parse_ctr() ) );
	}
	return nullptr;
}

var_base_t * push( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	v.push_back( fcd.args[ 1 ]->copy( fcd.args[ 1 ]->parse_ctr() ) );
	return nullptr;
}

var_base_t * pop( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	if( v.size() > 0 ) {
		VAR_DREF( v.back() );
		v.pop_back();
	}
	return nullptr;
}

var_base_t * pop_front( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	if( v.size() > 0 ) {
		VAR_DREF( v.front() );
		v.erase( v.begin() );
	}
	return nullptr;
}

var_base_t * front( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	return v.size() > 0 ? v.front() : nullptr;
}

var_base_t * back( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	return v.size() > 0 ? v.back() : nullptr;
}

var_base_t * len( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( ( int )AS_VEC( fcd.args[ 0 ] )->get().size(), fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * clear( vm_state_t & vm, func_call_data_t & fcd )
{
	AS_VEC( fcd.args[ 0 ] )->clear();
	return nullptr;
}

var_base_t * find( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	int loc = -1;
	size_t sz = v.size();
	for( size_t i = 0; i < sz; ++i ) {
		if( v[ i ]->to_str() == fcd.args[ 1 ]->to_str() ) {
			loc = i;
			break;
		}
	}
	return new var_int_t( loc, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * add( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > res;
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 0 ] )->get();
	for( auto & x : a ) {
		res.push_back( x->copy( fcd.args[ 1 ]->parse_ctr() ) );
	}
	for( auto & x : b ) {
		res.push_back( x->copy( fcd.args[ 1 ]->parse_ctr() ) );
	}
	return new var_vec_t( res, fcd.args[ 1 ]->parse_ctr() );
}

var_base_t * add_assn( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 0 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 1 ] )->get();
	for( auto & x : b ) {
		a.push_back( x->copy( fcd.args[ 0 ]->parse_ctr() ) );
	}
	return fcd.args[ 0 ];
}

var_base_t * eqv( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 0 ] )->get();
	if( a.size() != b.size() ) return vm.vars->get( "false" );
	for( auto i1 = a.begin(), i2 = b.begin(); i1 != a.end() && i2 != b.end(); ++i1, ++i2 ) {
		if( ( * i1 )->to_str() != ( * i2 )->to_str() ) return vm.vars->get( "false" );
	}
	return vm.vars->get( "true" );
}

var_base_t * nev( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 0 ] )->get();
	if( a.size() != b.size() ) return vm.vars->get( "true" );
	for( auto i1 = a.begin(), i2 = b.begin(); i1 != a.end() && i2 != b.end(); ++i1, ++i2 ) {
		if( ( * i1 )->to_str() != ( * i2 )->to_str() ) return vm.vars->get( "true" );
	}
	return vm.vars->get( "false" );
}

REGISTER_MODULE( vec )
{
	functions_t & vecfns = vm.typefuncs[ "vec" ];

	vecfns.add( { "append", 1, 1, { "vec" }, FnType::MODULE, { .modfn = append }, false } );
	vecfns.add( { "push", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = push }, false } );
	vecfns.add( { "pop", 0, 0, {}, FnType::MODULE, { .modfn = pop }, false } );
	vecfns.add( { "pop_front", 0, 0, {}, FnType::MODULE, { .modfn = pop_front }, false } );
	vecfns.add( { "front", 0, 0, {}, FnType::MODULE, { .modfn = front }, false } );
	vecfns.add( { "back", 0, 0, {}, FnType::MODULE, { .modfn = back }, false } );
	vecfns.add( { "len", 0, 0, {}, FnType::MODULE, { .modfn = len }, true } );
	vecfns.add( { "clear", 0, 0, {}, FnType::MODULE, { .modfn = clear }, false } );
	vecfns.add( { "find", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = find }, true } );

	vm.funcs.add( { "+", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = add }, true } );
	vm.funcs.add( { "+=", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = add_assn }, false } );

	vm.funcs.add( { "==", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = eqv }, false } );
	vm.funcs.add( { "!=", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = nev }, false } );
}
