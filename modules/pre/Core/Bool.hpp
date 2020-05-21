/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_BOOL_HPP
#define VM_MODULES_CORE_BOOL_HPP

#include "../../../src/VM/Core.hpp"

#define DECL_FUNC_ALLOC__BOOL( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		bool lhs = fcd.args[ 1 ]->to_bool();			\
		bool rhs = fcd.args[ 0 ]->to_bool();			\
		return TRUE_FALSE( lhs oper rhs );			\
	}

/*
 * comparison (equals and not equals) for booleans
 */
DECL_FUNC_ALLOC__BOOL( log_eq, == )
DECL_FUNC_ALLOC__BOOL( log_ne, != )

/*
 * boolean not (!true => false)
 */
var_base_t * not_operb( vm_state_t & vm, func_call_data_t & fcd )
{
	bool val = AS_BOOL( fcd.args[ 0 ] )->get();
	return TRUE_FALSE( !val );
}

/*
 * convert an object to boolean
 */
var_base_t * bool_create( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( fcd.args[ 0 ]->to_bool() );
}

/*
 * return a hash string from boolean
 */
var_base_t * hash_bool( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_str_t( fcd.args[ 0 ]->to_str() );
}

#endif // VM_MODULES_CORE_BOOL_HPP
