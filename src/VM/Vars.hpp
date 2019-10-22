/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_VARS_HPP
#define VM_VARS_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "Vars/Base.hpp"

class vars_t
{
	std::unordered_map< int, std::unordered_map< std::string, var_base_t * > > m_vars;
	size_t m_layer;
	std::vector< int > m_frozen_till;
public:
	vars_t();
	~vars_t();

	var_base_t * get( const std::string & var_name ) const;

	bool exists( const std::string & var_name ) const;

	void add( const std::string & var_name, var_base_t * const val );

	bool del( const std::string & var_name, var_base_t ** loc );

	void add_scope( const int count = 1 );
	void pop_scope( std::vector< void * > * locs, const int count = 1 );
	inline void freeze_till( const int till ) { m_frozen_till.push_back( till ); }
	inline void unfreeze() { m_frozen_till.pop_back(); }

	size_t layer_size() const;
};

#endif // VM_VARS_HPP