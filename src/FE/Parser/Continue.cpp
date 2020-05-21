/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Ethereal.hpp"

bool stmt_continue_t::bytecode( src_t & src ) const
{
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;
	src.bcode.push_back( { m_tok_ctr, line, col, IC_REM_SCOPE, { OP_INT, std::to_string( src.block_depth.back().back() - 1 ) } } );
	src.bcode.push_back( { m_tok_ctr, line, col, IC_JUMP, { OP_INT, "<continue-placeholder>" } } );
	return true;
}