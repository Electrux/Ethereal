/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_FLT_HPP
#define VM_MODULES_CORE_FLT_HPP

#include "../../src/VM/Core.hpp"

#define DECL_FUNC_ALLOC__FLT( name, oper, ret_type )			\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		auto & lhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		return new ret_type( lhs oper rhs );			\
	}

#define DECL_FUNC_BOOL__FLT( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		auto & lhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		return TRUE_FALSE( lhs oper rhs );			\
	}

#define DECL_FUNC_ASSN__FLT( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		lhs oper rhs;						\
		return fcd.args[ 0 ];					\
	}

DECL_FUNC_ALLOC__FLT( addf, +, var_flt_t )
DECL_FUNC_ALLOC__FLT( subf, -, var_flt_t )
DECL_FUNC_ALLOC__FLT( mulf, *, var_flt_t )
DECL_FUNC_ALLOC__FLT( divf, /, var_flt_t )

DECL_FUNC_ASSN__FLT( add_assnf, += )
DECL_FUNC_ASSN__FLT( sub_assnf, -= )
DECL_FUNC_ASSN__FLT( mul_assnf, *= )
DECL_FUNC_ASSN__FLT( div_assnf, /= )

DECL_FUNC_BOOL__FLT( eqf, == )
DECL_FUNC_BOOL__FLT( nef, != )
DECL_FUNC_BOOL__FLT( ltf, < )
DECL_FUNC_BOOL__FLT( lef, <= )
DECL_FUNC_BOOL__FLT( gtf, > )
DECL_FUNC_BOOL__FLT( gef, >= )

var_base_t * powerf( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & lhs = AS_FLT( fcd.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 0 ] )->get();
	var_flt_t * res = new var_flt_t( "0" );
	mpf_pow_ui( res->get().get_mpf_t(), lhs.get_mpf_t(), rhs.get_ui() );
	return res;
}

var_base_t * unary_subf( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & num = AS_FLT( fcd.args[ 0 ] )->get();
	return new var_flt_t( -num );
}

var_base_t * not_operf( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & num = AS_FLT( fcd.args[ 0 ] )->get();
	return TRUE_FALSE( !num );
}

var_base_t * flt( vm_state_t & vm, func_call_data_t & fcd )
{
	const std::string & flt_str = fcd.args[ 0 ]->to_str();
	return new var_flt_t( flt_str );
}

#endif // VM_MODULES_CORE_FLT_HPP
