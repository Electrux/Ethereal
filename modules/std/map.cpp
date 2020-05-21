/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../src/VM/Core.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::unordered_map< std::string, var_base_t * >::iterator map_iter_t;

class var_map_iter_t : public var_base_t
{
	map_iter_t m_it;
	var_map_t * m_map;
public:
	var_map_iter_t( var_map_t * map, map_iter_t it, bool inc_ref = true, const int src_idx = 0, const int parse_ctr = 0 );
	~var_map_iter_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	var_map_t * & get();
	map_iter_t & iter();
};
#define AS_MAP_ITER( x ) static_cast< var_map_iter_t * >( x )

var_map_iter_t::var_map_iter_t( var_map_t * map, map_iter_t it, bool inc_ref, const int src_idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, src_idx, parse_ctr ), m_it( it ), m_map( map )
{ if( inc_ref ) VAR_IREF( m_map ); it = map->get().begin(); }
var_map_iter_t::~var_map_iter_t() { VAR_DREF( m_map ); }

std::string var_map_iter_t::type_str() const { return "map_iter_t"; }
std::string var_map_iter_t::to_str() const
{
	return m_it->first;
}
mpz_class var_map_iter_t::to_int() const { return 1; }
bool var_map_iter_t::to_bool() const { return m_it != m_map->get().end(); }
var_base_t * var_map_iter_t::copy( const int src_idx, const int parse_ctr )
{
	return new var_map_iter_t( m_map, m_it, true, src_idx, parse_ctr );
}
void var_map_iter_t::assn( var_base_t * b )
{
	m_it = AS_MAP_ITER( b )->iter();
	VAR_DREF( m_map );
	var_map_t * map = AS_MAP_ITER( b )->get();
	VAR_IREF( map );
	m_map = map;
}
var_map_t * & var_map_iter_t::get() { return m_map; }
map_iter_t & var_map_iter_t::iter() { return m_it; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * insert( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_map< std::string, var_base_t * > & map = AS_MAP( fcd.args[ 0 ] )->get();
	std::string & key = AS_STR( fcd.args[ 1 ] )->get();
	if( map.find( key ) != map.end() ) {
		VAR_DREF( map[ key ] );
	}
	map[ key ] = fcd.args[ 2 ]->copy( fcd.src_idx, fcd.parse_ctr );
	return nullptr;
}

var_base_t * _delete( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_map< std::string, var_base_t * > & map = AS_MAP( fcd.args[ 0 ] )->get();
	std::string & key = AS_STR( fcd.args[ 1 ] )->get();
	if( map.find( key ) != map.end() ) {
		VAR_DREF( map[ key ] );
		map.erase( key );
		return vm.vars->get( "true" );
	}
	return vm.vars->get( "false" );
}

var_base_t * get( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_map< std::string, var_base_t * > & map = AS_MAP( fcd.args[ 0 ] )->get();
	std::string & key = AS_STR( fcd.args[ 1 ] )->get();
	if( map.find( key ) != map.end() ) {
		return map[ key ];
	}
	return vm.nil;
}

var_base_t * len( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( ( int )AS_MAP( fcd.args[ 0 ] )->get().size() );
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

var_base_t * iter( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_map_iter_t( AS_MAP( fcd.args[ 0 ] ), AS_MAP( fcd.args[ 0 ] )->get().begin() );
}

var_base_t * next( vm_state_t & vm, func_call_data_t & fcd )
{
	var_map_iter_t * it = AS_MAP_ITER( fcd.args[ 0 ] );
	std::unordered_map< std::string, var_base_t * > & map = it->get()->get();
	map_iter_t & pos = it->iter();
	if( pos == map.end() ) return vm.nil;
	VAR_IREF( pos->second );
	var_tuple_t * tup = new var_tuple_t( {
		new var_str_t( pos->first, fcd.parse_ctr ),
		pos->second
	} );
	++pos;
	return tup;
}

REGISTER_MODULE( map )
{
	functions_t & mapfns = vm.typefuncs[ "map" ];

	mapfns.add( { "_insert", 2, 2, { "str", "_any_" }, FnType::MODULE, { .modfn = insert }, false } );
	mapfns.add( { "_delete", 1, 1, { "str" }, FnType::MODULE, { .modfn = _delete }, false } );
	mapfns.add( { "get", 1, 1, { "str" }, FnType::MODULE, { .modfn = get }, false } );
	mapfns.add( { "len", 0, 0, {}, FnType::MODULE, { .modfn = len }, true } );
	mapfns.add( { "clear", 0, 0, {}, FnType::MODULE, { .modfn = clear }, false } );
	mapfns.add( { "_find", 1, 1, { "str" }, FnType::MODULE, { .modfn = find }, false } );
	mapfns.add( { "iter", 0, 0, {}, FnType::MODULE, { .modfn = iter }, true } );

	functions_t & mapiterfns = vm.typefuncs[ "map_iter_t" ];
	mapiterfns.add( { "next", 0, 0, {}, FnType::MODULE, { .modfn = next }, true } );
}
