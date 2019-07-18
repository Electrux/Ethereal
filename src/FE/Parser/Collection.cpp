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

bool stmt_collection_t::bytecode( src_t & src ) const
{
	if( m_vals ) m_vals->bytecode( src );

	src.bcode.push_back( { m_tok_ctr, m_line, m_col, m_is_map ? IC_BUILD_MAP : IC_BUILD_VEC,
	                       { OP_INT, std::to_string( m_is_map ? m_arg_count / 2 : m_arg_count ) } } );
	return true;
}