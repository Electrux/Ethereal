/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../src/VM/Core.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_vec_iter_t : public var_base_t
{
	int m_idx;
	var_vec_t * m_vec;
public:
	var_vec_iter_t( var_vec_t * vec, int idx = -1, const int parse_ctr = 0 );
	~var_vec_iter_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	var_vec_t * & get();
	int & pos();
};
#define AS_VEC_ITER( x ) static_cast< var_vec_iter_t * >( x )

var_vec_iter_t::var_vec_iter_t( var_vec_t * vec, int idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, parse_ctr ), m_idx( idx ), m_vec( vec )
{ VAR_IREF( m_vec ); }
var_vec_iter_t::~var_vec_iter_t() { VAR_DREF( m_vec ); }

std::string var_vec_iter_t::type_str() const { return "vec_iter_t"; }
std::string var_vec_iter_t::to_str() const
{
	return std::to_string( m_idx );
}
mpz_class var_vec_iter_t::to_int() const { return m_idx; }
bool var_vec_iter_t::to_bool() const { return m_idx >= 0 && m_idx < m_vec->get().size(); }
var_base_t * var_vec_iter_t::copy( const int parse_ctr )
{
	return new var_vec_iter_t( m_vec, m_idx, parse_ctr );
}
void var_vec_iter_t::assn( var_base_t * b )
{
	m_idx = AS_VEC_ITER( b )->pos();
	VAR_DREF( m_vec );
	var_vec_t * vec = AS_VEC_ITER( b )->get();
	VAR_IREF( vec );
	m_vec = vec;
}
var_vec_t * & var_vec_iter_t::get() { return m_vec; }
int & var_vec_iter_t::pos() { return m_idx; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * at( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	size_t sz = v.size();
	int idx = AS_INT( fcd.args[ 1 ] )->get().get_si();
	if( idx < 0 || ( size_t )idx >= sz ) {
		return vm.nil;
	}
	return v[ idx ];
}

var_base_t * append( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 0 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 1 ] )->get();
	for( auto & x : b ) {
		a.push_back( x->copy( fcd.parse_ctr ) );
	}
	return nullptr;
}

var_base_t * push( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	v.push_back( fcd.args[ 1 ]->copy( fcd.parse_ctr ) );
	return nullptr;
}

var_base_t * push_front( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & v = AS_VEC( fcd.args[ 0 ] )->get();
	v.insert( v.begin(), fcd.args[ 1 ]->copy( fcd.parse_ctr ) );
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
	return new var_int_t( ( int )AS_VEC( fcd.args[ 0 ] )->get().size() );
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
	return new var_int_t( loc );
}

var_base_t * iter( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_vec_iter_t( AS_VEC( fcd.args[ 0 ] ) );
}

var_base_t * add( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > res;
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 1 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 0 ] )->get();
	for( auto & x : a ) {
		res.push_back( x->copy( fcd.parse_ctr ) );
	}
	for( auto & x : b ) {
		res.push_back( x->copy( fcd.parse_ctr ) );
	}
	return new var_vec_t( res );
}

var_base_t * add_assn( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< var_base_t * > & a = AS_VEC( fcd.args[ 0 ] )->get();
	std::vector< var_base_t * > & b = AS_VEC( fcd.args[ 1 ] )->get();
	for( auto & x : b ) {
		a.push_back( x->copy( fcd.parse_ctr ) );
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

var_base_t * next( vm_state_t & vm, func_call_data_t & fcd )
{
	var_vec_iter_t * it = AS_VEC_ITER( fcd.args[ 0 ] );
	std::vector< var_base_t * > & vec = it->get()->get();
	int & pos = it->pos();
	++pos;
	if( pos >= vec.size() ) return vm.nil;
	return vec[ pos ];
}

REGISTER_MODULE( vec )
{
	functions_t & vecfns = vm.typefuncs[ "vec" ];

	vecfns.add( { "[]", 1, 1, { "int" }, FnType::MODULE, { .modfn = at }, false } );
	vecfns.add( { "at", 1, 1, { "int" }, FnType::MODULE, { .modfn = at }, false } );
	vecfns.add( { "append", 1, 1, { "vec" }, FnType::MODULE, { .modfn = append }, false } );
	vecfns.add( { "push", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = push }, false } );
	vecfns.add( { "push_front", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = push_front }, false } );
	vecfns.add( { "pop", 0, 0, {}, FnType::MODULE, { .modfn = pop }, false } );
	vecfns.add( { "pop_front", 0, 0, {}, FnType::MODULE, { .modfn = pop_front }, false } );
	vecfns.add( { "front", 0, 0, {}, FnType::MODULE, { .modfn = front }, false } );
	vecfns.add( { "back", 0, 0, {}, FnType::MODULE, { .modfn = back }, false } );
	vecfns.add( { "len", 0, 0, {}, FnType::MODULE, { .modfn = len }, true } );
	vecfns.add( { "clear", 0, 0, {}, FnType::MODULE, { .modfn = clear }, false } );
	vecfns.add( { "find", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = find }, true } );
	vecfns.add( { "iter", 0, 0, {}, FnType::MODULE, { .modfn = iter }, true } );

	vm.funcs.add( { "+", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = add }, true } );
	vm.funcs.add( { "+=", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = add_assn }, false } );

	vm.funcs.add( { "==", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = eqv }, false } );
	vm.funcs.add( { "!=", 2, 2, { "vec", "vec" }, FnType::MODULE, { .modfn = nev }, false } );

	functions_t & veciterfns = vm.typefuncs[ "vec_iter_t" ];
	veciterfns.add( { "next", 0, 0, {}, FnType::MODULE, { .modfn = next }, false } );
}
