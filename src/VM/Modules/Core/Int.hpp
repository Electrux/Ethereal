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

#include "../../Vars/Base.hpp"

#define DECL_FUNC_ALLOC__INT( name, oper, ret_type )				\
	var_base_t * name( std::vector< var_base_t * > & vars )			\
	{									\
		auto & lhs = AS_INT( vars[ 1 ] )->get();			\
		auto & rhs = AS_INT( vars[ 0 ] )->get();			\
		return new ret_type( lhs oper rhs, vars[ 0 ]->parse_ctr() );	\
	}

#define DECL_FUNC_ASSN__INT( name, oper )				\
	var_base_t * name( std::vector< var_base_t * > & vars )		\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_INT( vars[ 0 ] )->get();		\
		auto & rhs = AS_INT( vars[ 1 ] )->get();		\
		lhs oper rhs;						\
		return vars[ 1 ];					\
	}

DECL_FUNC_ALLOC__INT( add, +, var_int_t )
DECL_FUNC_ALLOC__INT( sub, -, var_int_t )
DECL_FUNC_ALLOC__INT( mul, *, var_int_t )
DECL_FUNC_ALLOC__INT( div, /, var_int_t )
DECL_FUNC_ALLOC__INT( mod, %, var_int_t )

DECL_FUNC_ASSN__INT( assn, = )
DECL_FUNC_ASSN__INT( add_assn, += )
DECL_FUNC_ASSN__INT( sub_assn, -= )
DECL_FUNC_ASSN__INT( mul_assn, *= )
DECL_FUNC_ASSN__INT( div_assn, /= )
DECL_FUNC_ASSN__INT( mod_assn, %= )

var_base_t * power( std::vector< var_base_t * > & vars )
{
	auto & lhs = AS_INT( vars[ 1 ] )->get();
	auto & rhs = AS_INT( vars[ 0 ] )->get();
	var_int_t * res = new var_int_t( "0", vars[ 0 ]->parse_ctr() );
	mpz_pow_ui( res->get().get_mpz_t(), lhs.get_mpz_t(), rhs.get_ui() );
	return res;
}

var_base_t * unary_sub( std::vector< var_base_t * > & vars )
{
	auto & num = AS_INT( vars[ 0 ] )->get();
	return new var_int_t( -num, vars[ 0 ]->parse_ctr() );
}

var_base_t * num( std::vector< var_base_t * > & vars )
{
	auto num_str = vars[ 0 ]->to_str();
	return new var_int_t( num_str, vars[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_INT_HPP