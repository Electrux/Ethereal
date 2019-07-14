/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_VM_STACK_HPP
#define VM_VM_STACK_HPP

#include <vector>

#include "Vars/Base.hpp"

class vm_stack_t
{
	std::vector< var_base_t * > m_vec;
public:
	vm_stack_t();
	~vm_stack_t();

	inline void push_back( var_base_t * val, const bool inc_ref = true )
	{
		if( inc_ref ) VAR_IREF( val );
		m_vec.push_back( val );
	}

	inline void pop_back( const bool dec_ref = true )
	{
		if( dec_ref ) VAR_DREF( m_vec.back() );
		m_vec.pop_back();
	}

	inline var_base_t * & back() { return m_vec.back(); }

	inline std::vector< var_base_t * > & get() { return m_vec; }

	inline size_t size() const { return m_vec.size(); }

	inline bool empty() const { return m_vec.empty(); }
};

#define VERIFY_STACK_MIN( sz )										\
	do {												\
		if( vm.stack->size() < sz ) {								\
			VM_FAIL( "expected vm stack size to be %d, but is %zu", vm.stack->size() );	\
			goto fail;									\
		}											\
	} while( 0 )

#endif // VM_VM_STACK_HPP