/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "ExecStack.hpp"

exec_stack_t::exec_stack_t() {}

exec_stack_t::~exec_stack_t()
{
	for( auto & val : m_vec ) {
		VAR_DREF( val );
	}
}