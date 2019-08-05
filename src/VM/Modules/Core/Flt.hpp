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

#include "../../Core.hpp"

#define DECL_FUNC_ALLOC__FLT( name, oper, ret_type )				\
	var_base_t * name( vm_state_t & vm )					\
	{									\
		auto & lhs = AS_FLT( vm.args[ 1 ] )->get();			\
		auto & rhs = AS_FLT( vm.args[ 0 ] )->get();			\
		return new ret_type( lhs oper rhs, vm.args[ 1 ]->parse_ctr() );	\
	}

#define DECL_FUNC_ASSN__FLT( name, oper )				\
	var_base_t * name( vm_state_t & vm )				\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_FLT( vm.args[ 0 ] )->get();		\
		auto & rhs = AS_FLT( vm.args[ 1 ] )->get();		\
		lhs oper rhs;						\
		return vm.args[ 0 ];					\
	}

DECL_FUNC_ALLOC__FLT( addf, +, var_flt_t )
DECL_FUNC_ALLOC__FLT( subf, -, var_flt_t )
DECL_FUNC_ALLOC__FLT( mulf, *, var_flt_t )
DECL_FUNC_ALLOC__FLT( divf, /, var_flt_t )

DECL_FUNC_ASSN__FLT( add_assnf, += )
DECL_FUNC_ASSN__FLT( sub_assnf, -= )
DECL_FUNC_ASSN__FLT( mul_assnf, *= )
DECL_FUNC_ASSN__FLT( div_assnf, /= )

DECL_FUNC_ALLOC__FLT( eqf, ==, var_bool_t )
DECL_FUNC_ALLOC__FLT( nef, !=, var_bool_t )
DECL_FUNC_ALLOC__FLT( ltf, <, var_bool_t )
DECL_FUNC_ALLOC__FLT( lef, <=, var_bool_t )
DECL_FUNC_ALLOC__FLT( gtf, >, var_bool_t )
DECL_FUNC_ALLOC__FLT( gef, >=, var_bool_t )

var_base_t * powerf( vm_state_t & vm )
{
	mpf_class & lhs = AS_FLT( vm.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( vm.args[ 0 ] )->get();
	var_flt_t * res = new var_flt_t( "0", vm.args[ 1 ]->parse_ctr() );
	mpf_pow_ui( res->get().get_mpf_t(), lhs.get_mpf_t(), rhs.get_ui() );
	return res;
}

var_base_t * unary_subf( vm_state_t & vm )
{
	mpf_class & num = AS_FLT( vm.args[ 0 ] )->get();
	return new var_flt_t( -num, vm.args[ 0 ]->parse_ctr() );
}

var_base_t * not_operf( vm_state_t & vm )
{
	mpf_class & num = AS_FLT( vm.args[ 0 ] )->get();
	return new var_bool_t( !num, vm.args[ 0 ]->parse_ctr() );
}

var_base_t * flt( vm_state_t & vm )
{
	const std::string & flt_str = vm.args[ 0 ]->to_str();
	return new var_flt_t( flt_str, vm.args[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_FLT_HPP
