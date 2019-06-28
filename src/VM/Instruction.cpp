/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Instruction.hpp"

const char * InstrCodeStrs[ _IC_LAST ] = {
	"IC_POP_STACK",
	"IC_LOAD_CONST",
	"IC_SAVE_VAR",
};