/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Ethereal.hpp"

bool stmt_break_t::bytecode( src_t & src ) const
{
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;
	// remove the second layer of loop so it can jump (unconditionally) to first layer
	src.bcode.push_back( { m_tok_ctr, line, col, IC_REM_SCOPE, { OP_NONE, "" }, false } );
	src.bcode.push_back( { m_tok_ctr, line, col, IC_JUMP, { OP_INT, "<break-placeholder>" }, false } );
	return true;
}