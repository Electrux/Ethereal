/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_FUNCTIONS_HPP
#define VM_FUNCTIONS_HPP

#include <vector>
#include <string>
#include <unordered_map>

class var_base_t;
class var_func_t;

typedef var_base_t * ( * cfn_t )( std::vector< var_base_t * > & args );
typedef var_func_t * efn_t;

enum FnType
{
	FT_CPP,
	FT_ET,
};

union Func
{
	cfn_t cfn; // function in c++
	efn_t efn; // function in ethereal
};

struct func_t
{
	std::string name;
	std::string module;
	int arg_count_min;
	int arg_count_max;
	std::vector< std::string > arg_types;
	FnType type;
	Func func;
	// all the functions with this value as true require the return value to be freed by the system's memory manager
	bool _requires_manual_return_free;
};

bool operator ==( const func_t & func1, const func_t & func2 );

class funcs_t
{
	std::vector< func_t > m_funcs;
	// uses mangled names and used for getting functions quickly
	// mangled style:
	// modname_fnname_argtypes
	std::unordered_map< std::string, const func_t * > m_cached_funcs;

	inline bool exists( const func_t & func )
	{
		return std::find( m_funcs.begin(), m_funcs.end(), func ) != m_funcs.end();
	}

	inline const func_t * get_cached_func( const std::string & mangled_name )
	{
		return m_cached_funcs.find( mangled_name ) != m_cached_funcs.end() ? m_cached_funcs[ mangled_name ] : nullptr;
	}

	inline const func_t * set_cached_func( const std::string & mangled_name, const func_t * func )
	{
		m_cached_funcs[ mangled_name ] = func;
		return func;
	}

public:
	funcs_t();
	~funcs_t();
	bool add( const func_t & func );
	bool add_vec( const std::vector< func_t > & funcs );

	bool del( const func_t & func );
	bool del_vec( const std::vector< func_t > & funcs );

	const func_t * get( const std::string & module, const std::string & name, const int arg_count,
			    const std::vector< std::string > & arg_types, const bool strict_module );
};

#endif // VM_FUNCTIONS_HPP