/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdlib>

#include "../FE/FS.hpp"

#include "ExecInternal.hpp"
#include "CallFunc.hpp"
#include "LoadFile.hpp"

#ifdef DEBUG_MODE
#include <chrono>
#endif

bool copy_data( var_base_t * a, var_base_t * b );

struct fn_blk_t
{
	int beg, end;
};

int exec_internal( vm_state_t & vm, long begin, long end, var_base_t * ret )
{
	src_t & src = * vm.srcstack.back();
	begin = begin == -1 ? 0 : begin;
	end = end == -1 ? src.bcode.size() : end;

	std::vector< fn_blk_t > fn_blks;
	std::vector< std::vector< std::string > > fn_args;

	func_call_data_t func_call_data;

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
			} else if( ins.oper.type == OP_NONE ) {
				val = vm.none;
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
		case IC_STORE_LOAD:
		case IC_STORE_NO_COPY:
		case IC_STORE_LOAD_NO_COPY: {
			std::string var;
			VERIFY_STACK_MIN( 2 );
			var = vm.stack->back()->to_str();
			vm.stack->pop_back();
			var_base_t * newval = vm.stack->back();
			if( vm.vars->exists( var, true ) ) {
				var_base_t * val = vm.vars->get( var );
				if( val->type() != newval->type() || ( ( val->type() == VT_STRUCT || val->type() == VT_CUSTOM ) &&
				    val->type_str() != newval->type_str() ) ) {
					VM_FAIL( "variable '%s' already declared at previous location, but with different data type (original: %s, new: %s)",
						 var.c_str(), val->type_str().c_str(), newval->type_str().c_str() );
					VM_FAIL_TOK_CTR( val->parse_ctr(), "original declared here" );
					goto fail;
				}

				if( !copy_data( val, newval ) ) {
					VM_FAIL( "copy semantics not implemented for variable type '%s'", val->type_str().c_str() );
					goto fail;
				}

				vm.stack->pop_back();
				if( ins.opcode == IC_STORE_LOAD ) {
					vm.stack->push_back( val );
				}
				break;
			}
			if( ins.opcode == IC_STORE_NO_COPY || ins.opcode == IC_STORE_LOAD_NO_COPY ) {
				VAR_IREF( newval );
				vm.vars->add( var, newval );
			} else {
				vm.vars->add( var, newval->copy( ins.parse_ctr ) );
			}
			vm.stack->pop_back();
			if( ins.opcode == IC_STORE_LOAD || ins.opcode == IC_STORE_LOAD_NO_COPY ) {
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
			if( var->type() != newval->type() || ( ( var->type() == VT_STRUCT || var->type() == VT_CUSTOM ) &&
			    var->type_str() != newval->type_str() ) ) {
				VM_FAIL( "assignment of an existing value expects same type, found lhs: %s and rhs: %s",
					 var->type_str().c_str(), newval->type_str().c_str() );
				goto fail;
			}

			if( !copy_data( var, newval ) ) {
				VM_FAIL( "copy semantics not implemented for variable type '%s'", var->type_str().c_str() );
				goto fail;
			}

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
		case IC_JUMP_TRUE: // fallthrough
		case IC_JUMP_FALSE: // fallthrough
		case IC_JUMP_TRUE_NO_POP: // fallthrough
		case IC_JUMP_FALSE_NO_POP: // fallthrough
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
				if( ins.opcode != IC_JUMP_TRUE_NO_POP && ins.opcode != IC_JUMP_FALSE_NO_POP ) vm.stack->pop_back();
				if( ( res  && ( ins.opcode == IC_JUMP_TRUE || ins.opcode == IC_JUMP_TRUE_NO_POP ) ) ||
				    ( !res && ( ins.opcode == IC_JUMP_FALSE || ins.opcode == IC_JUMP_FALSE_NO_POP ) ) ) res = true;
				else res = false;
			}
			if( res ) i = idx - 1;
			break;
		}
		case IC_LDMOD: {
			std::string module_name = ins.oper.val + ".so";
			std::string init_fn_str = ins.oper.val.substr( ins.oper.val.find_last_of( '/' ) + 4 );
			if( vm.dlib->load( module_name ) == nullptr ) goto fail;
			init_fnptr_t init_fn = ( init_fnptr_t ) vm.dlib->get( module_name, "init_" + init_fn_str );
			if( init_fn == nullptr ) {
				VM_FAIL( "failed to find init function '%s' in module '%s'\n", init_fn_str.c_str(), module_name.c_str() );
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
			int res = CallFunc( vm, func_call_data, i );
			if( res != E_OK ) goto fail;
			if( vm.exit_called ) return E_OK;
			break;
		}
		case IC_ATTR: {
			VERIFY_STACK_MIN( 2 );
			std::string attr = vm.stack->back()->to_str();
			vm.stack->pop_back();
			var_base_t * base = vm.stack->back();
			vm.stack->pop_back();
			if( base->type() != VT_ENUM && base->type() != VT_STRUCT && base->type() != VT_TUPLE ) {
				VM_FAIL( "expected one of 'enum', 'struct', or 'tuple' but found: %s", base->type_str().c_str() );
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
			} else if( base->type() == VT_STRUCT ) {
				std::unordered_map< std::string, var_base_t * > & val = AS_STRUCT( base )->get_val();
				if( val.find( attr ) == val.end() ) {
					VM_FAIL( "the struct '%s' does not contain attribute '%s'",
						 AS_STRUCT( base )->get_name().c_str(), attr.c_str() );
					goto fail;
				}
				vm.stack->push_back( val[ attr ] );
			} else if( base->type() == VT_TUPLE ) {
				std::vector< var_base_t * > & val = AS_TUPLE( base )->get();
				char * p = 0;
				strtol( attr.c_str(), & p, 10 );
				if( * p != 0 ) {
					VM_FAIL( "attribute for tuple must be a positive integer, found: %s", attr.c_str() );
					goto fail;
				}
				int idx = std::stoi( attr );
				if( idx < 0 || ( size_t )idx >= val.size() ) {
					VM_FAIL( "the tuple does not contain '%d' index, possible indices are: [0, %zu)",
						 idx, val.size() );
					goto fail;
				}
				vm.stack->push_back( val[ idx ] );
			}
			break;
		}
		case IC_BUILD_ENUM: // fallthrough
		case IC_BUILD_ENUM_MASK: {
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
			if( ins.opcode == IC_BUILD_ENUM_MASK ) {
				mpz_class mask = 1;
				while( count-- > 0 ) {
					map[ vm.stack->back()->to_str() ] = new var_int_t( mask, vm.stack->back()->parse_ctr() );
					vm.stack->pop_back();
					mask <<= 1;
				}
			} else {
				for( int j = 0; j < count; ++j ) {
					map[ vm.stack->back()->to_str() ] = new var_int_t( j, vm.stack->back()->parse_ctr() );
					vm.stack->pop_back();
				}
			}
			vm.vars->add( name, new var_enum_t( name, map, ins.parse_ctr ) );
			break;
		}
		case IC_BUILD_VEC: {
			int count = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )count );
			std::vector< var_base_t * > vec;
			while( count-- > 0 ) {
				vec.push_back( vm.stack->back()->copy( ins.parse_ctr ) );
				vm.stack->pop_back();
			}
			vm.stack->push_back( new var_vec_t( vec, ins.parse_ctr ), false );
			break;
		}
		case IC_BUILD_MAP: {
			int count = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )count * 2 );
			std::unordered_map< std::string, var_base_t * > map;
			while( count-- > 0 ) {
				std::string key = vm.stack->back()->to_str();
				vm.stack->pop_back();
				map[ key ] = vm.stack->back()->copy( ins.parse_ctr );
				vm.stack->pop_back();
			}
			vm.stack->push_back( new var_map_t( map, ins.parse_ctr ), false );
			break;
		}
		case IC_BUILD_STRUCT: {
			int count = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )count * 2 + 1 );
			std::string name = vm.stack->back()->to_str();
			if( vm.structs.find( name ) != vm.structs.end() ) {
				VM_FAIL( "struct by name '%s' is already defined", name.c_str() );
				goto fail;
			}
			vm.stack->pop_back();
			std::unordered_map< std::string, var_base_t * > map;
			std::vector< std::string > pos;
			while( count-- > 0 ) {
				std::string fname = vm.stack->back()->to_str();
				if( map.find( fname ) != map.end() ) {
					VM_FAIL_TOK_CTR( vm.stack->back()->parse_ctr(),
							 "field name '%s' has already been used before",
							 fname.c_str() );
					goto fail;
				}
				vm.stack->pop_back();
				var_base_t * fval = vm.stack->back()->copy( ins.parse_ctr );
				vm.stack->pop_back();
				map[ fname ] = fval;
				pos.insert( pos.begin(), fname );
			}
			vm.structs[ name ] = new var_struct_def_t( name, pos, map, ins.parse_ctr );
			break;
		}
		case IC_STRUCT_DECL: {
			int count = std::stoi( ins.oper.val );
			std::string name = vm.stack->back()->to_str();
			vm.stack->pop_back();
			if( vm.structs.find( name ) == vm.structs.end() ) {
				VM_FAIL( "struct by name '%s' does not exist", name.c_str() );
				goto fail;
			}
			var_struct_def_t * stdef = vm.structs[ name ];
			if( count < 0 || ( size_t )count > stdef->get_pos().size() ) {
				VM_FAIL( "struct '%s' expects at most %zu arguments (provided %d args)",
					 name.c_str(), stdef->get_pos().size(), count );
				goto fail;
			}
			var_struct_t * st = ( var_struct_t * )vm.structs[ name ]->copy( ins.parse_ctr );
			std::unordered_map< std::string, var_base_t * > & map = st->get_val();
			const std::vector< std::string > & pos = stdef->get_pos();
			for( int j = 0; j < count; ++j ) {
				if( map[ pos[ j ] ]->type() != vm.stack->back()->type() ) {
					VM_FAIL_TOK_CTR( vm.stack->back()->parse_ctr(),
							 "argument type mismatch: expected '%s', found '%s'",
							 map[ pos[ j ] ]->type_str().c_str(), vm.stack->back()->type_str().c_str() );
					goto fail;
				}
				if( !copy_data( map[ pos[ j ] ], vm.stack->back() ) ) {
					VM_FAIL( "copy semantics not implemented for variable type '%s'", map[ pos[ j ] ]->type_str().c_str() );
					goto fail;
				}
				vm.stack->pop_back();
			}
			vm.stack->push_back( st, false );
			break;
		}
		case IC_BLOCK_TILL: {
			int from = i + 1;
			int to = std::stoi( ins.oper.val );
			fn_blks.push_back( { from, to } );
			i = to - 1;
			break;
		}
		case IC_ARGS_TILL: {
			int till = std::stoi( ins.oper.val );
			std::vector< std::string > args;
			for( int j = till; j > i; --j ) {
				args.push_back( src.bcode[ j ].oper.val );
			}
			fn_args.push_back( args );
			if( till != -1 ) i = till;
			break;
		}
		case IC_BUILD_FN: // fallthrough
		case IC_BUILD_MFN: {
			fn_blk_t blk = fn_blks.back();
			fn_blks.pop_back();
			std::vector< std::string > args = fn_args.back();
			fn_args.pop_back();
			std::vector< std::string > member_ofs; 
			if( ins.opcode == IC_BUILD_MFN ) {
				VERIFY_STACK_MIN( 2 );
				int count = vm.stack->back()->to_int().get_si();
				vm.stack->pop_back();
				while( count-- > 0 ) {
					member_ofs.push_back( vm.stack->back()->to_str() );
					vm.stack->pop_back();
				}
			}
			std::string name = ins.oper.val;
			int args_count = args.size();
			for( auto & member_of : member_ofs ) {
				const function_t * fn = vm.typefuncs[ member_of ].get( name, args_count, {} );
				if( fn != nullptr ) {
					VM_FAIL( "function '%s::%s' already exists", member_of.c_str(), name.c_str() );
					goto fail;
				}
				vm.typefuncs[ member_of ].add( function_t{ name, args_count, args_count, args, FnType::LANG,
								           { .langfn = { src.id.c_str(), blk.beg, blk.end } },
									   false } );
			}
			if( member_ofs.size() == 0 ) {
				const function_t * fn = vm.funcs.get( name, args_count, {} );
				if( fn != nullptr ) {
					VM_FAIL( "function '%s' already exists", name.c_str() );
					goto fail;
				}
				vm.funcs.add( function_t{ name, args_count, args_count, args, FnType::LANG,
					                  { .langfn = { src.id.c_str(), blk.beg, blk.end } },
							  false } );
			}
			break;
		}
		case IC_RETURN: // fallthrough
		case IC_RETURN_EMPTY: {
			if( ins.oper.val != "0" ) {
				std::vector< void * > locs;
				vm.vars->pop_scope( & locs, std::stoi( ins.oper.val ) );
				for( auto & loc : locs ) VAR_DREF( loc );
			}
			if( ins.opcode == IC_RETURN_EMPTY ) {
				vm.stack->push_back( vm.none );
			}
			return E_OK;
		}
		case _IC_LAST: {}
		}
	}

	return E_OK;
fail:
	return E_VM_FAIL;
}

bool copy_data( var_base_t * a, var_base_t * b )
{
	if( a->impls_assn() ) {
		a->assn( b );
		return true;
	}

	return false;
}