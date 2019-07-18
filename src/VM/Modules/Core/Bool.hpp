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
		return new ret_type( lhs oper rhs, vars[ 0 ]->parse_ctr() );	\
	}

#define DECL_FUNC_ASSN__BOOL( name, oper )				\
	var_base_t * name( std::vector< var_base_t * > & vars )		\
	{	/* lhs = 0 because Right to Left associativity */	\
		bool lhs = vars[ 0 ]->to_bool();			\
		bool rhs = vars[ 1 ]->to_bool();			\
		lhs oper rhs;						\
		return vars[ 1 ];					\
	}

DECL_FUNC_ALLOC__BOOL( log_and, &&, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_or, ||, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_not, !=, var_bool_t )

#endif // VM_MODULES_CORE_BOOL_HPP
