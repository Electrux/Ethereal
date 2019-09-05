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
///////////////////////////////////////////////////////////// Class ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_evm_t : public var_base_t
{
	vm_state_t * m_vm;
	bool m_copied;
public:
	var_evm_t( vm_state_t * vm, const int parse_ctr = 0 );
	~var_evm_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	vm_state_t * & get();
};
#define AS_EVM( x ) static_cast< var_evm_t * >( x )

var_evm_t::var_evm_t( vm_state_t * vm, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, parse_ctr ), m_vm( vm ),
	  m_copied( false ) {}
var_evm_t::~var_evm_t()
{ if( !m_copied ) { m_vm->srcstack.pop_back(); delete m_vm; } }

std::string var_evm_t::type_str() const { return "evm_t"; }
std::string var_evm_t::to_str() const
{
	std::string str = "evm_t{";
	str += m_vm->srcstack.back()->name + "}";
	return str;
}
mpz_class var_evm_t::to_int() const { return m_vm->exit_status; }
bool var_evm_t::to_bool() const { return m_vm->exit_called; }
var_base_t * var_evm_t::copy( const int parse_ctr )
{
	m_copied = true;
	return new var_evm_t( m_vm, parse_ctr );
}
void var_evm_t::assn( var_base_t * b )
{
	var_evm_t * tmp = AS_EVM( b );
	if( !m_copied ) {
		delete tmp->m_vm;
	}

	m_vm = tmp->m_vm;
	tmp->m_copied = true;
	m_copied = false;
}
vm_state_t * & var_evm_t::get() { return m_vm; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * vm_create( vm_state_t & vm, func_call_data_t & fcd )
{
	int err = E_OK;
	src_t * src = new src_t( true );
	src->dir = vm.srcstack.back()->dir;
	src->name = fcd.args.size() > 0 ? fcd.args[ 0 ]->to_str() : "<repl>";
	vm_state_t * v = new vm_state_t();
	v->flags = vm.flags;
	v->srcstack.push_back( src );
	v->srcs[ src->name ] = src;

	var_base_t * prog = vm.vars->get( "__PROG__" );
	var_base_t * args = vm.vars->get( "args" );
	var_base_t * tru = vm.vars->get( "true" );
	var_base_t * fal = vm.vars->get( "false" );

	if( !set_init_mods( * v ) ) { err = E_VM_FAIL; goto fail; }

	VAR_IREF( prog );
	VAR_IREF( args );
	VAR_IREF( tru );
	VAR_IREF( fal );

	v->vars->add( "__PROG__", prog );
	v->vars->add( "args", args );
	v->vars->add( "true", tru );
	v->vars->add( "false", fal );

	VAR_IREF( v->nil );
	v->vars->add( "nil", v->nil );

	return new var_evm_t( v, 0 );
fail:
	delete v;
	return vm.nil;
}

var_base_t * vm_is_running( vm_state_t & vm, func_call_data_t & fcd )
{
	var_evm_t * v = AS_EVM( fcd.args[ 0 ] );
	return TRUE_FALSE( !v->get()->exit_called );
}

REGISTER_MODULE( vm )
{
	functions_t & vmfns = vm.typefuncs[ "_evm" ];
	vmfns.add( { "new", 0, 0, {}, FnType::MODULE, { .modfn = vm_create }, true } );

	functions_t & evmfns = vm.typefuncs[ "evm_t" ];
	evmfns.add( { "is_running", 0, 0, {}, FnType::MODULE, { .modfn = vm_is_running }, false } );
}