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

#ifdef DEBUG_MODE
#include <chrono>
#endif

int exec_internal( vm_state_t & vm, long begin, long end )
{
	src_t & src = * vm.srcstack.back();
	begin = begin == -1 ? 0 : begin;
	end = end == -1 ? src.bcode.size() : end;

#ifdef DEBUG_MODE
	typedef std::chrono::high_resolution_clock clk_t;
	std::chrono::time_point< clk_t > start = clk_t::now();
	std::chrono::time_point< clk_t > stamp;
#endif

	for( int i = begin; i < end; ++i ) {
		instr_t & ins = src.bcode[ i ];

#ifdef DEBUG_MODE
		stamp = clk_t::now();

		fprintf( stdout, "Current stack (operation [%d]: %s[%s]): ", i, InstrCodeStrs[ ins.opcode ], ins.oper.val.c_str() );
		//fprintf( stdout, "%f", std::chrono::duration_cast< std::chrono::duration< double, std::milli > >( stamp - start ).count() );
		for( auto & s : vm.stack->get() ) {
			fprintf( stdout, "%s ", s->to_str().c_str() );
		}
		start = stamp;
		fprintf( stdout, "\n" );
#endif
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
			VERIFY_STACK_MIN( 1 );
			var = vm.stack->back()->to_str();
			vm.stack->pop_back();
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
			vm.vars->add( var, newval->copy( ins.parse_ctr ) );
			vm.stack->pop_back();
			if( ins.opcode == IC_STORE_LOAD ) {
				vm.stack->push_back( vm.vars->get( var ) );
			}
			break;
		}
		case IC_STORE_STACK: // fallthrough
		case IC_STORE_LOAD_STACK: {
			var_base_t * var;
			VERIFY_STACK_MIN( 1 );
			if( vm.stack->back()->ref() == 1 ) {
				VM_FAIL( "storing a value requires the LHS to be an lvalue" );
				goto fail;
			}
			var = vm.stack->back();
			vm.stack->pop_back();
			var_base_t * newval = vm.stack->back();
			if( var->type() != newval->type() ) {
				VM_FAIL( "assignment of an existing value expects same type, found lhs: %s and rhs: %s",
						var->type_str().c_str(), newval->type_str().c_str() );
				goto fail;
			}
			VarType type = var->type();

			if( type == VT_INT ) AS_INT( var )->get() = AS_INT( newval )->get();
			else if( type == VT_FLT ) AS_FLT( var )->get() = AS_FLT( newval )->get();
			else if( type == VT_STR ) AS_STR( var )->get() = AS_STR( newval )->get();
			else if( type == VT_BOOL ) AS_BOOL( var )->get() = AS_BOOL( newval )->get();

			vm.stack->pop_back();
			if( ins.opcode == IC_STORE_LOAD_STACK ) {
				vm.stack->push_back( var );
			}
			break;
		}
		case IC_ADD_SCOPE: {
			int count = std::stoi( ins.oper.val );
			vm.vars->add_scope( count );
			break;
		}
		case IC_REM_SCOPE: {
			int count = std::stoi( ins.oper.val );
			std::vector< void * > locs;
			vm.vars->pop_scope( & locs, count );
			for( auto & loc : locs ) VAR_DREF( loc );
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
		case IC_ATTR: {
			VERIFY_STACK_MIN( 2 );
			std::string attr = vm.stack->back()->to_str();
			vm.stack->pop_back();
			var_base_t * base = vm.stack->back();
			vm.stack->pop_back();
			if( base->type() != VT_ENUM ) {
				VM_FAIL( "expected one of 'enum' but found: %s", base->type_str().c_str() );
				goto fail;
			}
			if( base->type() == VT_ENUM ) {
				std::unordered_map< std::string, var_int_t * > & val = AS_ENUM( base )->get_val();
				if( val.find( attr ) == val.end() ) {
					VM_FAIL( "the enum '%s' does not contain attribute '%s'",
						 AS_ENUM( base )->get_name().c_str(), attr.c_str() );
					goto fail;
				}
				vm.stack->push_back( val[ attr ] );
			}
			break;
		}
		case IC_SUBSCR: {
			VERIFY_STACK_MIN( 2 );
			// TODO: move the pop back for subscript at the end (here and in IC_ATTR)
			var_base_t * sub = vm.stack->back();
			bool manual_del_sub = false;
			if( sub->ref() == 1 ) { manual_del_sub = true; vm.stack->pop_back( false ); }
			else vm.stack->pop_back();
			var_base_t * var = vm.stack->back();
			vm.stack->pop_back();
			if( var == nullptr ) {
				VM_FAIL( "variable '%s' does not exist", ins.oper.val.c_str() );
				goto tmp_fail;
			}
			if( var->type() != VT_STR && var->type() != VT_VEC && var->type() != VT_MAP ) {
				VM_FAIL( "expected one of 'str', 'vec', or 'map' but found: %s", var->type_str().c_str() );
				goto tmp_fail;
			}
			if( var->type() == VT_STR || var->type() == VT_VEC ) {
				if( sub->type() != VT_INT ) {
					VM_FAIL( "subscript expression must be of integer type, but found: '%s'",
						 sub->type_str().c_str() );
					goto tmp_fail;
				}
				int idx = sub->to_int().get_si();
				if( var->type() == VT_STR ) {
					std::string & val = AS_STR( var )->get();
					if( idx < 0 || idx >= ( int )val.size() ) {
						VM_FAIL( "index can only be between [0, %zu), but is: %d", val.size(), idx );
						goto tmp_fail;
					}
					vm.stack->push_back( new var_str_t( std::string( 1, val[ idx ] ), ins.parse_ctr ), false );
				} else if( var->type() == VT_VEC ) {
					std::vector< var_base_t * > & val = AS_VEC( var )->get();
					if( idx < 0 || idx >= ( int )val.size() ) {
						VM_FAIL( "index can only be between [0, %zu), but is: %d", val.size(), idx );
						goto tmp_fail;
					}
					vm.stack->push_back( val[ idx ] );
				}
			} else if( var->type() == VT_MAP ) {
				std::unordered_map< std::string, var_base_t * > & val = AS_MAP( var )->get();
				if( sub->type() != VT_STR && sub->type() != VT_INT ) {
					VM_FAIL( "subscript expression must be of string or int type, but found: '%s'",
						 sub->type_str().c_str() );
					goto tmp_fail;
				}
				const std::string key = sub->to_str();
				if( val.find( key ) == val.end() ) {
					VM_FAIL( "map does not contain a key '%s'", key.c_str() );
					goto tmp_fail;
				}
				vm.stack->push_back( val.at( key ) );
			}
			if( manual_del_sub ) VAR_DREF( sub );
			break;
tmp_fail:
			if( manual_del_sub ) VAR_DREF( sub );
			goto fail;
		}
		case IC_BUILD_ENUM: {
			std::string name = vm.stack->back()->to_str();
			vm.stack->pop_back();
			if( vm.vars->exists( name, true ) ) {
				var_base_t * val = vm.vars->get( name );
				VM_FAIL( "variable '%s' already declared at another location" );
				VM_FAIL_TOK_CTR( val->parse_ctr(), "original declared here" );
				goto fail;
			}
			int count = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )count );
			std::unordered_map< std::string, var_int_t * > map;
			for( int i = 0; i < count; ++i ) {
				map[ vm.stack->back()->to_str() ] = new var_int_t( i, vm.stack->back()->parse_ctr() );
				vm.stack->pop_back();
			}
			vm.vars->add( name, new var_enum_t( name, map, ins.parse_ctr ) );
			break;
		}
		case IC_BUILD_VEC: {
			const int count = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )count );
			std::vector< var_base_t * > vec;
			for( int i = 0; i < count; ++i ) {
				vec.push_back( vm.stack->back()->copy( ins.parse_ctr ) );
				vm.stack->pop_back();
			}
			vm.stack->push_back( new var_vec_t( vec, ins.parse_ctr ), false );
			break;
		}
		case IC_BUILD_MAP: {
			const int count = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )count * 2 );
			std::unordered_map< std::string, var_base_t * > map;
			for( int i = 0; i < count; ++i ) {
				std::string key = vm.stack->back()->to_str();
				vm.stack->pop_back();
				map[ key ] = vm.stack->back()->copy( ins.parse_ctr );
				vm.stack->pop_back();
			}
			vm.stack->push_back( new var_map_t( map, ins.parse_ctr ), false );
			break;
		}
		case _IC_LAST: {}
		}
	}

	return E_OK;
fail:
	return E_VM_FAIL;
}