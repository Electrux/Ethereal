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
	std::vector< int > m_none_locs;
public:
	vm_stack_t();
	~vm_stack_t();

	inline void push_back( var_base_t * val, const bool inc_ref = true )
	{
		if( val->type() == VT_NONE ) m_none_locs.push_back( m_vec.size() );
		if( inc_ref ) VAR_IREF( val );
		m_vec.push_back( val );
	}

	inline void pop_back( const bool dec_ref = true )
	{
		if( m_vec.back()->type() == VT_NONE ) m_none_locs.pop_back();
		if( dec_ref ) VAR_DREF( m_vec.back() );
		m_vec.pop_back();
	}

	inline var_base_t * & back() { return m_vec.back(); }

	inline std::vector< var_base_t * > & get() { return m_vec; }

	inline size_t size() const { return m_vec.size(); }

	inline bool empty() const { return m_vec.empty(); }

	inline bool has_none( const int range ) { return m_none_locs.size() > 0 && ( int )m_vec.size() - range <= m_none_locs.back(); }
};

#define VERIFY_STACK_MIN( sz )																	\
	do {																			\
		if( vm.stack->size() < sz ) {															\
			VM_FAIL( "expected vm stack size to be %zu, but is %zu (verify that a function is returning what it should)",				\
				 sz, vm.stack->size() );													\
			goto fail;																\
		}																		\
		if( vm.stack->has_none( sz ) ) {														\
			VM_FAIL( "Stack contains value of None which cannot be used" );										\
			goto fail;																\
		}																		\
	} while( 0 )

#endif // VM_VM_STACK_HPP