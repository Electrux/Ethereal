/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../Ethereal.hpp"
#include "ExecInternal.hpp"

#define _STRINGIZE(x) #x
#define STRINGIFY(x) _STRINGIZE(x)

var_base_t * exec_internal( vm_state_t & vmstate )
{
	var_base_t * res = nullptr;

	vm_src_t & curr_src = vmstate.srcs.back();

	for( int i = curr_src.begin; i < curr_src.end; ++i ) {
		auto & ins = curr_src.src.bcode[ i ];
		fprintf( stdout, "instr %s: %s (%s)\n", InstrCodeStrs[ ins.opcode ], ins.oper.val, TypeStrs[ ins.oper.type ] );
	}

	return 0;
}