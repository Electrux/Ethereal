/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_BOOL_HPP
#define VM_MODULES_CORE_BOOL_HPP

#include "../../Vars/Base.hpp"

#define DECL_FUNC_ALLOC__BOOL( name, oper, ret_type )				\
	var_base_t * name( std::vector< var_base_t * > & vars )			\
	{									\
		bool lhs = vars[ 1 ]->to_bool();				\
		bool rhs = vars[ 0 ]->to_bool();				\
		return new ret_type( lhs oper rhs, vars[ 1 ]->parse_ctr() );	\
	}

DECL_FUNC_ALLOC__BOOL( log_and, &&, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_or, ||, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_eq, ==, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_ne, !=, var_bool_t )

var_base_t * not_operb( std::vector< var_base_t * > & vars )
{
	bool val = AS_BOOL( vars[ 0 ] )->get();
	return new var_bool_t( !val, vars[ 0 ]->parse_ctr() );
}

var_base_t * bool_create( std::vector< var_base_t * > & vars )
{
	return new var_bool_t( vars[ 0 ]->to_bool(), vars[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_BOOL_HPP
