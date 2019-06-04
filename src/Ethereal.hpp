/*
	Copyright (c) 2018, Electrux
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

#include "Core.hpp"
#include "Lexer.hpp"

typedef std::vector< tok_t > toks_t;

struct src_t
{
	std::string name;
	toks_t toks;
	std::vector< std::string > src;
	bool is_main_src;

	src_t( const bool _is_main_src = false );
};

typedef std::unordered_map< std::string, src_t > srcs_t;

// the ethereal main struct
struct eth_t
{
	srcs_t srcs;

	std::vector< std::string > src_stack;

	eth_t();
};

#endif // ETHEREAL_HPP