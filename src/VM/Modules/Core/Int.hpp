/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_INT_HPP
#define VM_MODULES_CORE_INT_HPP

#include "../../Core.hpp"

#define DECL_FUNC_ALLOC__INT( name, oper, ret_type )				\
	var_base_t * name( vm_state_t & vm )					\
	{									\
		auto & lhs = AS_INT( vm.args[ 1 ] )->get();			\
		auto & rhs = AS_INT( vm.args[ 0 ] )->get();			\
		return new ret_type( lhs oper rhs, vm.args[ 1 ]->parse_ctr() );	\
	}

#define DECL_FUNC_BOOL__INT( name, oper )						\
	var_base_t * name( vm_state_t & vm )						\
	{										\
		auto & lhs = AS_INT( vm.args[ 1 ] )->get();				\
		auto & rhs = AS_INT( vm.args[ 0 ] )->get();				\
		return lhs oper rhs ? vm.vars->get( "true" ) : vm.vars->get( "false" );	\
	}

#define DECL_FUNC_ASSN__INT( name, oper )				\
	var_base_t * name( vm_state_t & vm )				\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_INT( vm.args[ 0 ] )->get();		\
		auto & rhs = AS_INT( vm.args[ 1 ] )->get();		\
		lhs oper rhs;						\
		return vm.args[ 0 ];					\
	}

DECL_FUNC_ALLOC__INT( add, +, var_int_t )
DECL_FUNC_ALLOC__INT( sub, -, var_int_t )
DECL_FUNC_ALLOC__INT( mul, *, var_int_t )
DECL_FUNC_ALLOC__INT( div, /, var_int_t )
DECL_FUNC_ALLOC__INT( mod, %, var_int_t )

DECL_FUNC_ASSN__INT( add_assn, += )
DECL_FUNC_ASSN__INT( sub_assn, -= )
DECL_FUNC_ASSN__INT( mul_assn, *= )
DECL_FUNC_ASSN__INT( div_assn, /= )
DECL_FUNC_ASSN__INT( mod_assn, %= )

DECL_FUNC_BOOL__INT( eqi, == )
DECL_FUNC_BOOL__INT( nei, != )
DECL_FUNC_BOOL__INT( lti, < )
DECL_FUNC_BOOL__INT( lei, <= )
DECL_FUNC_BOOL__INT( gti, > )
DECL_FUNC_BOOL__INT( gei, >= )

var_base_t * power( vm_state_t & vm )
{
	mpz_class & lhs = AS_INT( vm.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( vm.args[ 0 ] )->get();
	var_int_t * res = new var_int_t( "0", vm.args[ 1 ]->parse_ctr() );
	mpz_pow_ui( res->get().get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return res;
}

var_base_t * unary_sub( vm_state_t & vm )
{
	mpz_class & num = AS_INT( vm.args[ 0 ] )->get();
	return new var_int_t( -num, vm.args[ 0 ]->parse_ctr() );
}

var_base_t * not_oper( vm_state_t & vm )
{
	mpz_class & num = AS_INT( vm.args[ 0 ] )->get();
	return !num ? vm.vars->get( "true" ) : vm.vars->get( "false" );
}

var_base_t * num( vm_state_t & vm )
{
	return new var_int_t( vm.args[ 0 ]->to_int(), vm.args[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_INT_HPP