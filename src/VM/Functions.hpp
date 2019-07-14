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
struct vm_state_t;

typedef var_base_t * ( * modfnptr_t )( std::vector< var_base_t * > & args );
//typedef var_func_t * langfnptr_t;

enum FnType
{
	MODULE,
	LANG,
};

union Func
{
	modfnptr_t modfn;
//	langfnptr_t langfn;
};

struct function_t
{
	std::string name;
	int arg_count_min;
	int arg_count_max;
	std::vector< std::string > arg_types;
	FnType type;
	Func func;
	// all the functions with this value as true require the return value to be freed by the system's memory manager
	bool _requires_manual_return_free;
};

#define REGISTER_MODULE( name )				\
	extern "C" void init_##name( vm_state_t & vm )

typedef void ( * init_fnptr_t )( vm_state_t & vm );

bool operator ==( const function_t & func1, const function_t & func2 );

class functions_t
{
	std::vector< function_t > m_funcs;
	// uses mangled names and used for getting functions quickly
	// mangled style:
	// modname_fnname_argtypes
	std::unordered_map< std::string, const function_t * > m_cached_funcs;

	inline bool exists( const function_t & func )
	{
		return std::find( m_funcs.begin(), m_funcs.end(), func ) != m_funcs.end();
	}

	inline const function_t * get_cached_func( const std::string & mangled_name )
	{
		return m_cached_funcs.find( mangled_name ) != m_cached_funcs.end() ? m_cached_funcs[ mangled_name ] : nullptr;
	}

	inline const function_t * set_cached_func( const std::string & mangled_name, const function_t * func )
	{
		m_cached_funcs[ mangled_name ] = func;
		return func;
	}

public:
	functions_t();
	~functions_t();
	bool add( const function_t & func );
	bool add_vec( const std::vector< function_t > & funcs );

	bool del( const function_t & func );
	bool del_vec( const std::vector< function_t > & funcs );

	const function_t * get( const std::string & name, const int arg_count,
				const std::vector< std::string > & arg_types );
};

#endif // VM_FUNCTIONS_HPP