/*
  Copyright (c) 2019, Electrux
  All rights reserved.
  Using the BSD 3-Clause license for the project,
  main LICENSE file resides in project's root directory.
  Please read that file and understand the license terms
  before using or altering the project.
*/

#include <algorithm>

#include "Vars/Base.hpp"
#include "Functions.hpp"

static bool compare_arg_types( const std::vector< std::string > & ats1, const std::vector< std::string > & ats2 );

bool operator ==( const function_t & func1, const function_t & func2 )
{
	return func1.name == func2.name && func1.arg_count_min == func2.arg_count_min
		&& func1.arg_count_max <= func2.arg_count_max && compare_arg_types( func1.arg_types, func2.arg_types );
}

////////////////////////////////// PRIVATE MEMBER FUNCTIONS ///////////////////////////////////

////////////////////////////////// PUBLIC MEMBER FUNCTIONS ////////////////////////////////////

functions_t::functions_t() {}

functions_t::~functions_t()
{
	for( auto & fninf : m_funcs ) {
		//if( fninf.type == LANG ) VAR_DREF( fninf.func.langfn );
	}
}

bool functions_t::add( const function_t & func )
{
	if( exists( func ) ) {
		fprintf( stderr, "Function: %s::%s already exists\n", func.module.c_str(), func.name.c_str() );
		return false;
	}
	m_funcs.push_back( func );
	return true;
}

bool functions_t::add_vec( const std::vector< function_t > & funcs )
{
	for( auto & func : funcs ) {
		if( exists( func ) ) {
			fprintf( stderr, "Function: %s::%s already exists\n", func.module.c_str(), func.name.c_str() );
			return false;
		}
		m_funcs.push_back( func );
	}
	return true;
}

bool functions_t::del( const function_t & func )
{
	auto loc = std::find( m_funcs.begin(), m_funcs.end(), func );
	if( loc != m_funcs.end() ) {
		m_funcs.erase( loc );
		return true;
	}
	fprintf( stderr, "Function: %s::%s does not exist\n", func.module.c_str(), func.name.c_str() );
	return false;
}

bool functions_t::del_vec( const std::vector< function_t > & funcs )
{
	for( auto & func : funcs ) {
		if( !del( func ) ) return false;
	}
	return true;
}

const function_t * functions_t::get( const std::string & module, const std::string & name, const int arg_count,
				     const std::vector< std::string > & arg_types, const bool strict_module )
{
	std::string mangled_name = module + "_" + name;
	for( auto & arg_type : arg_types ) {
		mangled_name += "_" + arg_type;
	}
	const function_t * fnptr = get_cached_func( mangled_name );
	if( fnptr != nullptr ) return fnptr;

	for( auto & func : m_funcs ) {
		if( func.name == name && ( func.module == module || ( !strict_module && func.module == "<global>" ) )
		    && arg_count >= func.arg_count_min
		    && ( arg_count <= func.arg_count_max || func.arg_count_max == -1 )
		    && ( func.type == LANG || compare_arg_types( func.arg_types, arg_types ) ) ) {
			return set_cached_func( mangled_name, & func );
		}
	}
	return nullptr;
}

static bool compare_arg_types( const std::vector< std::string > & ats1, const std::vector< std::string > & ats2 )
{
	if( ats2.empty() ) return true;
	auto it = ats2.begin();
	for( auto & at1 : ats1 ) {
		if( at1 == "_whatever_" ) return true;
		// only last argument can be optional (_any_) (more than one argument will be done by _whatever_)
		if( it == ats2.end() ) return at1 == "_any_";
		if( at1 != "_any_" && at1 != * it ) return false;
		++it;
	}
	return true;
}