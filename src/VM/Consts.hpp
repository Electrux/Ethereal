/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_CONSTS_HPP
#define VM_CONSTS_HPP

#include <string>
#include <unordered_map>

#include "Instruction.hpp"

class consts_t
{
	std::unordered_map< std::string, var_base_t * > m_consts;
public:
	consts_t();
	~consts_t();
	var_base_t * get( const instr_t & ins );
};

#endif // VM_CONSTS_HPP