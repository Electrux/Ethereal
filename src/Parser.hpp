/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_HPP
#define PARSER_HPP

#include "Parser/Stmts.hpp"

typedef std::vector< stmt_base_t * > parse_tree_t;

parse_tree_t * parse( src_t & src, parse_helper_t * pre_ph = nullptr, const GrammarTypes parent = GRAM_INVALID,
				      const int end = -1 );

#endif // PARSER_HPP