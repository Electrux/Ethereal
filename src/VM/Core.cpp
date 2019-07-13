/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Core.hpp"

vm_state_t::vm_state_t() : stack( new vm_stack_t() ) {}

vm_state_t::~vm_state_t()
{
	delete stack;
	for( auto & src : srcmap ) {
		delete src.second;
	}
}