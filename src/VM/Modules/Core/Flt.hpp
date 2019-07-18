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

#include "../../Vars/Base.hpp"

#define DECL_FUNC_ALLOC__FLT( name, oper, ret_type )				\
	var_base_t * name( std::vector< var_base_t * > & vars )			\
	{									\
		auto & lhs = AS_FLT( vars[ 1 ] )->get();			\
		auto & rhs = AS_FLT( vars[ 0 ] )->get();			\
		return new ret_type( lhs oper rhs, vars[ 0 ]->parse_ctr() );	\
	}

#define DECL_FUNC_ASSN__FLT( name, oper )				\
	var_base_t * name( std::vector< var_base_t * > & vars )		\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_FLT( vars[ 0 ] )->get();		\
		auto & rhs = AS_FLT( vars[ 1 ] )->get();		\
		lhs oper rhs;						\
		return vars[ 1 ];					\
	}

DECL_FUNC_ALLOC__FLT( addf, +, var_flt_t )
DECL_FUNC_ALLOC__FLT( subf, -, var_flt_t )
DECL_FUNC_ALLOC__FLT( mulf, *, var_flt_t )
DECL_FUNC_ALLOC__FLT( divf, /, var_flt_t )

DECL_FUNC_ASSN__FLT( add_assnf, += )
DECL_FUNC_ASSN__FLT( sub_assnf, -= )
DECL_FUNC_ASSN__FLT( mul_assnf, *= )
DECL_FUNC_ASSN__FLT( div_assnf, /= )

var_base_t * powerf( std::vector< var_base_t * > & vars )
{
	auto & lhs = AS_FLT( vars[ 1 ] )->get();
	auto & rhs = AS_INT( vars[ 0 ] )->get();
	var_flt_t * res = new var_flt_t( "0", vars[ 0 ]->parse_ctr() );
	mpf_pow_ui( res->get().get_mpf_t(), lhs.get_mpf_t(), rhs.get_ui() );
	return res;
}

var_base_t * unary_subf( std::vector< var_base_t * > & vars )
{
	auto & num = AS_FLT( vars[ 0 ] )->get();
	return new var_flt_t( -num, vars[ 0 ]->parse_ctr() );
}

var_base_t * flt( std::vector< var_base_t * > & vars )
{
	const std::string & flt_str = vars[ 0 ]->to_str();
	return new var_flt_t( flt_str, vars[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_FLT_HPP
