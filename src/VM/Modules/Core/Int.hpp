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
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )		\
	{									\
		auto & lhs = AS_INT( fcd.args[ 1 ] )->get();			\
		auto & rhs = AS_INT( fcd.args[ 0 ] )->get();			\
		return new ret_type( lhs oper rhs, fcd.args[ 1 ]->parse_ctr() );\
	}

#define DECL_FUNC_BOOL__INT( name, oper )						\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )			\
	{										\
		auto & lhs = AS_INT( fcd.args[ 1 ] )->get();				\
		auto & rhs = AS_INT( fcd.args[ 0 ] )->get();				\
		return lhs oper rhs ? vm.vars->get( "true" ) : vm.vars->get( "false" );	\
	}

#define DECL_FUNC_ASSN__INT( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_INT( fcd.args[ 0 ] )->get();		\
		auto & rhs = AS_INT( fcd.args[ 1 ] )->get();		\
		lhs oper rhs;						\
		return fcd.args[ 0 ];					\
	}

#define DECL_FUNC_BIT__INT( name, oper )					\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )		\
	{	/* lhs = 0 because Right to Left associativity */		\
		auto & lhs = AS_INT( fcd.args[ 0 ] )->get();			\
		auto & rhs = AS_INT( fcd.args[ 1 ] )->get();			\
		return new var_int_t( lhs oper rhs, fcd.args[ 0 ]->parse_ctr() );\
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

DECL_FUNC_BIT__INT( andi, & )
DECL_FUNC_BIT__INT( ori, | )

var_base_t * power( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & lhs = AS_INT( fcd.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 0 ] )->get();
	var_int_t * res = new var_int_t( "0", fcd.args[ 1 ]->parse_ctr() );
	mpz_pow_ui( res->get().get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return res;
}

var_base_t * lshift( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & lhs = AS_INT( fcd.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 0 ] )->get();
	var_int_t * res = new var_int_t( "0", fcd.args[ 1 ]->parse_ctr() );
	mpz_mul_2exp( res->get().get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return res;
}

var_base_t * rshift( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & lhs = AS_INT( fcd.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 0 ] )->get();
	var_int_t * res = new var_int_t( "0", fcd.args[ 1 ]->parse_ctr() );
	mpz_div_2exp( res->get().get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return res;
}

var_base_t * lshift_assn( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & lhs = AS_INT( fcd.args[ 0 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 1 ] )->get();
	mpz_mul_2exp( lhs.get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return fcd.args[ 0 ];
}

var_base_t * rshift_assn( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & lhs = AS_INT( fcd.args[ 0 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 1 ] )->get();
	mpz_div_2exp( lhs.get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return fcd.args[ 0 ];
}

var_base_t * unary_sub( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & num = AS_INT( fcd.args[ 0 ] )->get();
	return new var_int_t( -num, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * not_oper( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & num = AS_INT( fcd.args[ 0 ] )->get();
	return !num ? vm.vars->get( "true" ) : vm.vars->get( "false" );
}

var_base_t * not_oper_bitwise( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & num = AS_INT( fcd.args[ 0 ] )->get();
	return new var_int_t( ~num, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * num( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( fcd.args[ 0 ]->to_int(), fcd.args[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_INT_HPP