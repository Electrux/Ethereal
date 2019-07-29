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

#include "../../Core.hpp"

#define DECL_FUNC_ALLOC__BOOL( name, oper, ret_type )				\
	var_base_t * name( vm_state_t & vm )					\
	{									\
		bool lhs = vm.args[ 1 ]->to_bool();				\
		bool rhs = vm.args[ 0 ]->to_bool();				\
		return new ret_type( lhs oper rhs, vm.args[ 1 ]->parse_ctr() );	\
	}

DECL_FUNC_ALLOC__BOOL( log_and, &&, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_or, ||, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_eq, ==, var_bool_t )
DECL_FUNC_ALLOC__BOOL( log_ne, !=, var_bool_t )

var_base_t * not_operb( vm_state_t & vm )
{
	bool val = AS_BOOL( vm.args[ 0 ] )->get();
	return new var_bool_t( !val, vm.args[ 0 ]->parse_ctr() );
}

var_base_t * bool_create( vm_state_t & vm )
{
	return new var_bool_t( vm.args[ 0 ]->to_bool(), vm.args[ 0 ]->parse_ctr() );
}

#endif // VM_MODULES_CORE_BOOL_HPP
