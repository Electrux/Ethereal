/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <unordered_set>

#include "../Core.hpp"

class var_set_t : public var_base_t
{
	std::unordered_set< std::string > m_set;
public:
	var_set_t( std::unordered_set< std::string > set, const int parse_ctr );
	~var_set_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	std::unordered_set< std::string > & get();
};
#define AS_SET( x ) static_cast< var_set_t * >( x )

var_set_t::var_set_t( std::unordered_set< std::string > set, const int parse_ctr )
	: var_base_t( VT_CUSTOM, parse_ctr, true ), m_set( set ) {}
var_set_t::~var_set_t() {}

std::string var_set_t::type_str() const { return "_set_t"; }
std::string var_set_t::to_str() const
{
	std::string str = "set_t{";
	for( auto & s : m_set ) {
		str += s + ", ";
	}
	// remove the extra commas
	if( m_set.size() > 0 ) {
		str.pop_back();
		str.pop_back();
	}
	str += "}";
	return str;
}
mpz_class var_set_t::to_int() const { return m_set.size(); }
bool var_set_t::to_bool() const { return m_set.size() != 0; }
var_base_t * var_set_t::copy( const int parse_ctr )
{
	return new var_set_t( m_set, parse_ctr );
}
void var_set_t::assn( var_base_t * b )
{
	m_set = AS_SET( b )->get();
}
std::unordered_set< std::string > & var_set_t::get() { return m_set; }

var_base_t * set_create( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_set_t( {}, 0 );
}

var_base_t * set_insert( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_set< std::string > & s = AS_SET( fcd.args[ 0 ] )->get();
	for( size_t i = 1; i < fcd.args.size(); ++i ) {
		s.insert( fcd.args[ i ]->to_str() );
	}
	return nullptr;
}

var_base_t * set_erase( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_set< std::string > & s = AS_SET( fcd.args[ 0 ] )->get();
	for( size_t i = 1; i < fcd.args.size(); ++i ) {
		s.erase( fcd.args[ i ]->to_str() );
	}
	return nullptr;
}

var_base_t * set_contains( vm_state_t & vm, func_call_data_t & fcd )
{
	std::unordered_set< std::string > & s = AS_SET( fcd.args[ 0 ] )->get();
	return s.find( fcd.args[ 1 ]->to_str() ) != s.end() ? vm.vars->get( "true" ) : vm.vars->get( "false" );
}

REGISTER_MODULE( set )
{
	vm.funcs.add( { "set_create", 0, 0, {}, FnType::MODULE, { .modfn = set_create }, true } );

	functions_t & st = vm.typefuncs[ "_set_t" ];
	st.add( { "insert", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = set_insert }, false } );
	st.add( { "erase", 1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = set_erase }, false } );
	st.add( { "contains", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = set_contains }, false } );
}