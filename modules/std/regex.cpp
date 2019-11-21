/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <regex>

#include "../../src/VM/Core.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Class ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_regex_t : public var_base_t
{
	std::string m_str;
	std::regex m_expr;
public:
	var_regex_t( const std::string & str, const int src_idx = 0, const int parse_ctr = 0 );
	~var_regex_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	const std::string & get_str();
	const std::regex & get_regex();
};
#define AS_REGEX( x ) static_cast< var_regex_t * >( x )

var_regex_t::var_regex_t( const std::string & str, const int src_idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, src_idx, parse_ctr ), m_str( str ), m_expr( str ) {}
var_regex_t::~var_regex_t() {}

std::string var_regex_t::type_str() const { return "regex_t"; }
std::string var_regex_t::to_str() const
{
	return "regex_t{" + m_str + "}";
}
mpz_class var_regex_t::to_int() const { return m_str.size(); }
bool var_regex_t::to_bool() const { return !m_str.empty(); }
var_base_t * var_regex_t::copy( const int src_idx, const int parse_ctr )
{
	return new var_regex_t( m_str, src_idx, parse_ctr );
}
void var_regex_t::assn( var_base_t * b )
{
	m_str = AS_REGEX( b )->m_str;
	m_expr = AS_REGEX( b )->m_expr;
}
const std::string & var_regex_t::get_str() { return m_str; }
const std::regex & var_regex_t::get_regex() { return m_expr; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * regex_create( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_regex_t( fcd.args[ 1 ]->to_str() );
}

var_base_t * regex_match( vm_state_t & vm, func_call_data_t & fcd )
{
	if( AS_REGEX( fcd.args[ 0 ] )->get_str().empty() ) return vm.vars->get( "true" );
	bool res = std::regex_match( fcd.args[ 1 ]->to_str(), AS_REGEX( fcd.args[ 0 ] )->get_regex() );
	return TRUE_FALSE( res );
}

var_base_t * regex_empty( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( AS_REGEX( fcd.args[ 0 ] )->get_str().empty() );
}

REGISTER_MODULE( regex )
{
	functions_t & regex_mod = vm.typefuncs[ "_regex_t" ];
	regex_mod.add( { "build", 1, 1, { "str" }, FnType::MODULE, { .modfn = regex_create }, true } );

	functions_t & regex = vm.typefuncs[ "regex_t" ];
	regex.add( { "match", 1, 1, { "str" }, FnType::MODULE, { .modfn = regex_match }, false } );
	regex.add( { "empty", 0, 0, {}, FnType::MODULE, { .modfn = regex_empty }, false } );
}