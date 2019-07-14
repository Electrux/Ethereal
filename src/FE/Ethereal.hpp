/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef ETHEREAL_HPP
#define ETHEREAL_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "Parser.hpp"
#include "../VM/Instructions.hpp"
#include "../VM/Vars.hpp"

struct src_t
{
	std::string name;
	std::vector< std::string > code;
	toks_t toks;
	parse_tree_t * ptree;
	bool is_main_src;
	bool bcode_as_const;

	// for VM
	bytecode_t bcode;
	vars_t vars;
	function_t funcs;

	src_t( const bool _is_main_src );
	~src_t();
};

typedef std::unordered_map< std::string, src_t * > srcs_t;

#endif // ETHEREAL_HPP