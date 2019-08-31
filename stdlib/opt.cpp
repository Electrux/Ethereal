/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../src/VM/Core.hpp"

class var_opt_t : public var_base_t
{
	var_base_t * m_val;
public:
	var_opt_t( var_base_t * val, const int parse_ctr = 0 );
	~var_opt_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void clear();
	void assn( var_base_t * b );
	var_base_t * & get();
};
#define AS_OPT( x ) static_cast< var_opt_t * >( x )

var_opt_t::var_opt_t( var_base_t * val, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, parse_ctr ), m_val( val )
{ if( m_val ) VAR_IREF( m_val ); }
var_opt_t::~var_opt_t() { if( m_val ) VAR_DREF( m_val ); }

std::string var_opt_t::type_str() const { return "opt_t"; }
std::string var_opt_t::to_str() const
{
	std::string str = "opt_t{";
	if( m_val == nullptr ) str += "null}";
	else str += m_val->to_str() + "}";
	return str;
}
mpz_class var_opt_t::to_int() const { return m_val == nullptr ? 0 : 1; }
bool var_opt_t::to_bool() const { return m_val != nullptr; }
var_base_t * var_opt_t::copy( const int parse_ctr )
{
	return new var_opt_t( m_val, parse_ctr );
}
void var_opt_t::clear() { if( m_val ) { VAR_DREF( m_val ); m_val = nullptr; } }
void var_opt_t::assn( var_base_t * b )
{
	VAR_DREF( m_val );
	m_val = AS_OPT( b )->get();
	VAR_IREF( m_val );
}
var_base_t * & var_opt_t::get() { return m_val; }

var_base_t * opt_create( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_opt_t( nullptr, 0 );
}

var_base_t * opt_empty( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( AS_OPT( fcd.args[ 0 ] )->get() == nullptr );
}

var_base_t * opt_clear( vm_state_t & vm, func_call_data_t & fcd )
{
	AS_OPT( fcd.args[ 0 ] )->clear();
	return nullptr;
}

var_base_t * opt_set( vm_state_t & vm, func_call_data_t & fcd )
{
	AS_OPT( fcd.args[ 0 ] )->clear();
	AS_OPT( fcd.args[ 0 ] )->get() = fcd.args[ 1 ]->copy( fcd.parse_ctr );
	return AS_OPT( fcd.args[ 0 ] )->get();
}

var_base_t * opt_get( vm_state_t & vm, func_call_data_t & fcd )
{
	return AS_OPT( fcd.args[ 0 ] )->get();
}

REGISTER_MODULE( opt )
{
	vm.funcs.add( { "opt_new", 0, 0, {}, FnType::MODULE, { .modfn = opt_create }, true } );

	functions_t & opt = vm.typefuncs[ "opt_t" ];
	opt.add( { "empty", 0, 0, {}, FnType::MODULE, { .modfn = opt_empty }, false } );
	opt.add( { "clear", 0, 0, {}, FnType::MODULE, { .modfn = opt_clear }, false } );
	opt.add( { "set",   1, 1, { "_any_" }, FnType::MODULE, { .modfn = opt_set }, false } );
	opt.add( { "get",   0, 0, {}, FnType::MODULE, { .modfn = opt_get }, false } );
}