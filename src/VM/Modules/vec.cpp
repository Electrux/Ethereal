/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../Vars/Base.hpp"
#include "../Core.hpp"

var_base_t * append( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & a = AS_VEC( vars[ 0 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( vars[ 1 ] )->get();
	for( auto & x : b ) {
		a.push_back( x->copy( vars[ 0 ]->parse_ctr() ) );
	}
	return nullptr;
}

var_base_t * push( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & v = AS_VEC( vars[ 0 ] )->get();
	v.push_back( vars[ 1 ]->copy( vars[ 1 ]->parse_ctr() ) );
	return nullptr;
}

var_base_t * pop( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & v = AS_VEC( vars[ 0 ] )->get();
	if( v.size() > 0 ) {
		VAR_DREF( v.back() );
		v.pop_back();
	}
	return nullptr;
}

var_base_t * pop_front( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & v = AS_VEC( vars[ 0 ] )->get();
	if( v.size() > 0 ) {
		VAR_DREF( v.front() );
		v.erase( v.begin() );
	}
	return nullptr;
}

var_base_t * front( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & v = AS_VEC( vars[ 0 ] )->get();
	return v.size() > 0 ? v.front() : nullptr;
}

var_base_t * back( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & v = AS_VEC( vars[ 0 ] )->get();
	return v.size() > 0 ? v.back() : nullptr;
}

var_base_t * len( std::vector< var_base_t * > & vars )
{
	return new var_int_t( ( int )AS_VEC( vars[ 0 ] )->get().size(), vars[ 0 ]->parse_ctr() );
}

var_base_t * clear( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & v = AS_VEC( vars[ 0 ] )->get();
	for( auto & e : v ) VAR_DREF( e );
	v.clear();
	return nullptr;
}

var_base_t * add( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > res;
	std::vector< var_base_t * > & a = AS_VEC( vars[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( vars[ 0 ] )->get();
	for( auto & x : a ) {
		res.push_back( x->copy( vars[ 1 ]->parse_ctr() ) );
	}
	for( auto & x : b ) {
		res.push_back( x->copy( vars[ 1 ]->parse_ctr() ) );
	}
	return new var_vec_t( res, vars[ 1 ]->parse_ctr() );
}

var_base_t * add_assn( std::vector< var_base_t * > & vars )
{
	std::vector< var_base_t * > & a = AS_VEC( vars[ 0 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( vars[ 1 ] )->get();
	for( auto & x : b ) {
		a.push_back( x->copy( vars[ 0 ]->parse_ctr() ) );
	}
	return vars[ 0 ];
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

	vm.funcs.add( { "+", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = add }, true } );
	vm.funcs.add( { "+=", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = add_assn }, false } );
}
