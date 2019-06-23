/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_DYN_LIB_HPP
#define VM_DYN_LIB_HPP

#include <string>
#include <unordered_map>

/* Wrapper class for dlfcn.h library */
class dyn_lib_t
{
	std::unordered_map< std::string, void * > m_handles;
public:
	dyn_lib_t();
	~dyn_lib_t();
	void * load( const std::string & file );
	void unload( const std::string & file );
	void * get_sym( const std::string & file, const std::string & sym );
};

#endif // VM_DYN_LIB_HPP