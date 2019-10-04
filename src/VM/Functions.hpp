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
#include <algorithm>
#include <unordered_map>

class var_base_t;
struct vm_state_t;
struct func_call_data_t;

typedef var_base_t * ( * modfnptr_t )( vm_state_t & vm, func_call_data_t & fcd );
struct langfn_t
{
	const char * src;
	int beg, end;
};

enum FnType
{
	MODULE,
	LANG,
};

union Func
{
	modfnptr_t modfn;
	langfn_t langfn;
};

struct function_t
{
	std::string name;
	int arg_count_min;
	int arg_count_max;
	std::vector< std::string > arg_types;
	FnType type;
	Func func;
	bool manual_res_free;
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
	// fnname_argtypes
	std::unordered_map< std::string, int > m_cached_funcs;

	inline bool exists( const function_t & func )
	{
		return std::find( m_funcs.begin(), m_funcs.end(), func ) != m_funcs.end();
	}

	inline const function_t * get_cached_func( const std::string & mangled_name )
	{
		return m_cached_funcs.find( mangled_name ) != m_cached_funcs.end() ? & m_funcs[ m_cached_funcs[ mangled_name ] ] : nullptr;
	}

	inline const function_t * set_cached_func( const std::string & mangled_name, const int fid )
	{
		m_cached_funcs[ mangled_name ] = fid;
		return & m_funcs[ fid ];
	}

public:
	functions_t();
	~functions_t();
	bool add( const function_t & func );
	bool add_vec( const std::vector< function_t > & funcs );

	bool del( const function_t & func );
	bool del_vec( const std::vector< function_t > & funcs );

	bool exists_name( const std::string & name );

	const function_t * get( const std::string & name, const int arg_count,
				const std::vector< std::string > & arg_types );
	const std::vector< function_t * > get_all_by_name( const std::string & name );
};

#endif // VM_FUNCTIONS_HPP