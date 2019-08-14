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

#define DECL_FUNC_ALLOC__BOOL( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		bool lhs = fcd.args[ 1 ]->to_bool();			\
		bool rhs = fcd.args[ 0 ]->to_bool();			\
		return TRUE_FALSE( lhs oper rhs );			\
	}

DECL_FUNC_ALLOC__BOOL( log_and, && )
DECL_FUNC_ALLOC__BOOL( log_or, || )
DECL_FUNC_ALLOC__BOOL( log_eq, == )
DECL_FUNC_ALLOC__BOOL( log_ne, != )

var_base_t * not_operb( vm_state_t & vm, func_call_data_t & fcd )
{
	bool val = AS_BOOL( fcd.args[ 0 ] )->get();
	return TRUE_FALSE( !val );
}

var_base_t * bool_create( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( fcd.args[ 0 ]->to_bool() );
}

#endif // VM_MODULES_CORE_BOOL_HPP
