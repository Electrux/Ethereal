/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../FE/FS.hpp"

#include "ExecInternal.hpp"
#include "CallFunc.hpp"
#include "LoadFile.hpp"

int exec_internal( vm_state_t & vm, long begin, long end )
{
	src_t & src = * vm.srcstack.back();
	begin = begin == -1 ? 0 : begin;
	end = end == -1 ? src.bcode.size() : end;

	for( int i = begin; i < end; ++i ) {
		instr_t & ins = src.bcode[ i ];
/*
		fprintf( stdout, "Current stack (operation [%d]: %s[%s]): ", i, InstrCodeStrs[ ins.opcode ], ins.oper.val.c_str() );
		for( auto & s : vm.stack->get() ) {
			fprintf( stdout, "%s[%d] ", s->to_str().c_str(), s->ref() );
		}
		fprintf( stdout, "\n" );
*/
		switch( ins.opcode ) {
		case IC_PUSH: {
			var_base_t * val = nullptr;
			if( ins.oper.type == OP_STR ) {
				val = vm.vars->get( ins.oper.val );
			} else {
				val = vm.consts->get( ins.oper.val, ins.oper.type, ins.parse_ctr );
			}
			if( val == nullptr ) {
				VM_FAIL( "%s '%s' does not exist", ins.oper.type == OP_STR ? "variable" : "constant",
					 ins.oper.val.c_str() );
				goto fail;
			}
			vm.stack->push_back( val );
			break;
		}
		case IC_POP: {
			if( vm.stack->size() == 0 ) {
				VM_FAIL( "cannot pop from vm stack since it is already empty" );
				goto fail;
			}
			vm.stack->pop_back();
			break;
		}
		case IC_STORE: // fallthrough
		case IC_STORE_LOAD: {
			std::string var;
			if( ins.oper.type == OP_NONE ) {
				VERIFY_STACK_MIN( 1 );
				var = vm.stack->back()->to_str();
				vm.stack->pop_back();
			} else {
				// TODO:
			}
			var_base_t * newval = vm.stack->back();
			if( vm.vars->exists( var, true ) ) {
				var_base_t * val = vm.vars->get( var );
				if( val->type() != newval->type() ) {
					VM_FAIL( "variable '%s' already declared at previous location, but with different data type" );
					VM_FAIL_TOK_CTR( val->parse_ctr(), "original declared here" );
					goto fail;
				}
				VarType type = val->type();

				if( type == VT_INT ) AS_INT( val )->get() = AS_INT( newval )->get();
				else if( type == VT_FLT ) AS_FLT( val )->get() = AS_FLT( newval )->get();
				else if( type == VT_STR ) AS_STR( val )->get() = AS_STR( newval )->get();
				else if( type == VT_BOOL ) AS_BOOL( val )->get() = AS_BOOL( newval )->get();
				// TODO:

				vm.stack->pop_back();
				if( ins.opcode == IC_STORE_LOAD ) {
					vm.stack->push_back( val );
				}
				break;
			}
			vm.vars->add( var, newval->copy() );
			vm.stack->pop_back();
			if( ins.opcode == IC_STORE_LOAD ) {
				vm.stack->push_back( vm.vars->get( var ) );
			}
			break;
		}
		case IC_ADD_SCOPE: {
			int count = std::stoi( ins.oper.val );
			while( count-- > 0 ) vm.vars->add_scope();
			break;
		}
		case IC_REM_SCOPE: {
			int count = std::stoi( ins.oper.val );
			while( count-- > 0 ) {
				std::vector< void * > locs;
				vm.vars->pop_scope( & locs );
				for( auto & loc : locs ) VAR_DREF( loc );
			}
			break;
		}
		case IC_JUMP_FALSE: // fallthrough
		case IC_JUMP_TRUE: // fallthrough
		case IC_JUMP: {
			bool res = true;
			const int idx = std::stoi( ins.oper.val );
			if( idx < begin || idx > end ) {
				VM_FAIL( "bytecode location %d exceeds the beginning and end points: [%d,%d)",
					 idx, begin, end );
				goto fail;
			}
			if( ins.opcode != IC_JUMP ) {
				VERIFY_STACK_MIN( 1 );
				res = vm.stack->back()->to_bool();
				vm.stack->pop_back();
				if( ( res && ins.opcode == IC_JUMP_TRUE ) || ( !res && ins.opcode == IC_JUMP_FALSE ) ) res = true;
				else res = false;
			}
			if( res ) i = idx - 1;
			break;
		}
		case IC_LDMOD: {
			std::string module_name = ins.oper.val + ".so";
			if( ins.oper.val.front() != '~' && ins.oper.val.front() != '/' && ins.oper.val.front() != '.' ) {
				module_name = STRINGIFY( BUILD_PREFIX_DIR ) "/lib/ethereal/lib" + ins.oper.val + ".so";
			}
			if( !fexists( module_name ) ) {
				VM_FAIL( "failed to locate module file '%s'", module_name.c_str() );
				goto fail;
			}
			if( vm.dlib->load( module_name ) == nullptr ) goto fail;
			init_fnptr_t init_fn = ( init_fnptr_t ) vm.dlib->get( module_name, "init_" + ins.oper.val );
			if( init_fn == nullptr ) {
				VM_FAIL( "failed to find init function in module '%s'\n", module_name.c_str() );
				goto fail;
			}
			init_fn( vm );
			break;
		}
		case IC_IMPORT: {
			const int args = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )args );
			std::string alias = "";
			if( args > 1 ) {
				alias = vm.stack->back()->to_str();
				vm.stack->pop_back();
			}
			const std::string file = vm.stack->back()->to_str();
			vm.stack->pop_back();

			int ret = load_src( vm, file, alias, ins );

			if( ret != E_OK ) {
				VM_FAIL( "could not import '%s', see the error above; aborting",
					 file.c_str() );
				goto fail;
			}
			break;
		}
		case IC_FN_CALL: // fallthrough
		case IC_MFN_CALL: {
			int res = CallFunc( vm, i );
			if( res != E_OK ) goto fail;
			break;
		}
		case _IC_LAST: {}
		}
	}

	return E_OK;
fail:
	return E_VM_FAIL;
}