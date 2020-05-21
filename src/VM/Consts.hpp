/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_CONSTS_HPP
#define VM_CONSTS_HPP

#include <string>
#include <unordered_map>

#include "Instructions.hpp"
#include "Vars/Base.hpp"

typedef std::unordered_map< int, std::unordered_map< std::string, var_base_t * > > constmap_t;

class consts_t
{
	constmap_t m_consts;
public:
	consts_t();
	~consts_t();
	var_base_t * get( const std::string & name, const OperTypes type, const int tok_ctr );
};

#endif // VM_CONSTS_HPP