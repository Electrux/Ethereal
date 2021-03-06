/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "ExecInternal.hpp"
#include "CallFunc.hpp"

static std::string args_types_to_string( const std::vector< var_base_t * > & args, bool is_mem );
static void disp_possible_funcs( vm_state_t & vm, const std::string & fn, const std::string & mem_of );
static void disp_fn_vec( const std::vector< function_t * > & fns );

int CallFunc( vm_state_t & vm, func_call_data_t & fcd, const int ins_ctr )
{
	src_t & src = * vm.srcstack.back();
	instr_t & ins = src.bcode[ ins_ctr ];
	fcd.bcodectr = ins_ctr;

	fcd.arg_types.clear();
	fcd.rem_locs.clear();
	fcd.src_idx = src.id;
	fcd.parse_ctr = ins.parse_ctr;
	int var_args_count = 0;
	var_base_t * member = nullptr;
	const function_t * fn;
	modfnptr_t mfnptr = nullptr;
	const langfn_t * lfnptr = nullptr;
	res_t< var_base_t * > res = { 0, nullptr };

	// this is so early on in the function because if the execution goes to 'fail',
	// it will call unfreeze() which will pop the last element unconditionally
	// which will cause segfault if there is nothing in the m_frozen_till vector in
	// the first place
	vm.vars->freeze_till( vm.vars->layer_size() );

	fcd.args_count = std::stoi( ins.oper.val );
	// + 1 for function name
	VERIFY_STACK_MIN( ( size_t )fcd.args_count + 1 );
	if( vm.stack->back()->type() != VT_STR ) {
		VM_FAIL( "expected a name (identifier) for function call, found '%s'",
			 vm.stack->back()->to_str().c_str() );
		goto fail;
	}
	fcd.fn_name = vm.stack->back()->to_str();
	vm.stack->pop_back();
	// fetch args
	for( int i = 0; i < fcd.args_count; ++i ) {
		var_base_t * back = vm.stack->back();
		if( back->type() == VarType::VT_VEC && AS_VEC( back )->is_var_arg() ) {
			std::vector< var_base_t * > & vec = AS_VEC( back )->get();
			// minus 1 because vec replaces the argument, not appends to it
			var_args_count += vec.size() - 1;
			for( auto & e : vec ) {
				VAR_IREF( e );
				fcd.arg_types.push_back( e->type_str() );
				fcd.args.push_back( e );
			}
			vm.stack->pop_back();
		} else {
			fcd.arg_types.push_back( back->type_str() );
			fcd.args.push_back( back );
			vm.stack->pop_back( false );
		}
	}
	fcd.args_count += var_args_count;

	if( ins.opcode == IC_MFN_CALL ) {
		member = vm.stack->back();
		vm.stack->pop_back( false );
		fcd.args.insert( fcd.args.begin(), member );
		fn = vm.typefuncs[ member->type_str() ].get( fcd.fn_name, fcd.args_count, fcd.arg_types );
		if( fn == nullptr ) {
			fn = vm.typefuncs[ "_any_" ].get( fcd.fn_name, fcd.args_count, fcd.arg_types );
		}
	} else {
		// fetch the function pointer from Functions
		fn = vm.funcs.get( fcd.fn_name, fcd.args_count, fcd.arg_types );
	}
	if( fn == nullptr ) {
		VM_FAIL( "%sfunction with name '%s' %sand arg count %d (%s) does not exist",
			 ins.opcode == IC_MFN_CALL ? "member " : "",fcd.fn_name.c_str(),
			 ins.opcode == IC_MFN_CALL ? ( "for type '" + member->type_str() + "' " ).c_str() : "",
			 fcd.args_count, args_types_to_string( fcd.args, member ).c_str() );
		disp_possible_funcs( vm, fcd.fn_name, ins.opcode == IC_MFN_CALL ? member->type_str() : "" );
		goto fail;
	}
	if( fn->type == FnType::MODULE ) mfnptr = fn->func.modfn;
	else lfnptr = & fn->func.langfn;
	if( mfnptr == nullptr && lfnptr == nullptr ) {
		VM_FAIL( "function with name '%s' is null", fcd.fn_name.c_str() );
		goto fail;
	}

	// execute the function
	res.code = E_OK;
	if( mfnptr ) {
		res.data = mfnptr( vm, fcd );
		// for lang function, args are moved to vars' new layer which is popped at the end
		// member is also added to args and hence erased
		for( auto & arg : fcd.args ) VAR_DREF( arg );
	} else {
		int use_layer = vm.vars->layer_size() + 1;
		vm.srcstack.push_back( vm.srcs[ lfnptr->src ] );
		if( member ) {
			fcd.args.erase( fcd.args.begin() );
			vm.vars->add_with_layer( "self", member, use_layer );
		}
		for( size_t i = 0; i < fn->arg_count_min; ++i ) {
			vm.vars->add_with_layer( fn->arg_types[ i ], fcd.args[ i ], use_layer );
		}
		if( fn->arg_count_max == -1 ) {
			std::vector< var_base_t * > va;
			for( size_t i = fn->arg_count_min; i < fcd.args.size(); ++i ) {
				va.push_back( fcd.args[ i ] );
			}
			vm.vars->add_with_layer( "__VA__", new var_vec_t( va, fcd.src_idx, fcd.parse_ctr, true ), use_layer );
		}
		fcd.args.clear();
		res.code = exec_internal( vm, lfnptr->beg, lfnptr->end, res.data );
		vm.srcstack.pop_back();
		if( res.code != E_OK ) {
			VM_FAIL( "function '%s' failed to execute properly", fcd.fn_name.c_str() );
			goto fail;
		}
	}
	if( mfnptr ) {
		if( res.data == nullptr ) {
			vm.stack->push_back( vm.none );
		} else {
			res.data->set_src_idx( fcd.src_idx );
			res.data->set_parse_ctr( fcd.parse_ctr );
			vm.stack->push_back( res.data, !fn->manual_res_free || res.data->type() == VT_NIL );
		}
	}

	vm.vars->unfreeze();
	for( auto & rll : fcd.rem_locs ) VAR_DREF( rll );
	fcd.args.clear();
	return E_OK;
fail:
	vm.vars->unfreeze();
	for( auto & arg : fcd.args ) VAR_DREF( arg );
	fcd.args.clear();
	for( auto & rll : fcd.rem_locs ) VAR_DREF( rll );
	return E_VM_FAIL;
}

static std::string args_types_to_string( const std::vector< var_base_t * > & args, bool is_mem )
{
	std::string res = "";
	size_t size = args.size();
	for( size_t i = 0 + is_mem; i < size; ++i ) {
		res += args[ i ]->type_str();
		if( i < size - 1 ) res += ", ";
	}
	return res;
}

static void disp_possible_funcs( vm_state_t & vm, const std::string & fn, const std::string & mem_of )
{
	std::vector< function_t * > fns = vm.funcs.get_all_by_name( fn );
	if( fns.size() > 0 ) {
		fprintf( stderr, "Other possible functions with same name are:\n" );
		disp_fn_vec( fns );
	}

	if( mem_of == "" ) {
		for( auto & type : vm.typefuncs ) {
			std::vector< function_t * > mfns = type.second.get_all_by_name( fn );
			if( mfns.size() > 0 ) {
				fprintf( stderr, "Possible member functions for type '%s' with same name are:\n",
					 type.first.c_str() );
				disp_fn_vec( mfns );
			}
		}
	} else {
		std::vector< function_t * > mfns = vm.typefuncs[ mem_of ].get_all_by_name( fn );
		if( mfns.size() > 0 ) {
			fprintf( stderr, "Possible member functions for type '%s' with same name are:\n",
				 mem_of.c_str() );
			disp_fn_vec( mfns );
		}
	}
}

static void disp_fn_vec( const std::vector< function_t * > & fns )
{
	for( auto & fn : fns ) {
		fprintf( stderr, "-> %s with arg count: [%d, %d] ", fn->name.c_str(), fn->arg_count_min, fn->arg_count_max );
		if( fn->arg_count_min > 0 && fn->arg_types.size() < 1 ) {
			fprintf( stderr, "(...)\n" );
		} else {
			fprintf( stderr, "(" );
			for( auto & type : fn->arg_types ) {
				fprintf( stderr, "%s, ", type.c_str() );
			}
			if( fn->arg_count_max == -1 ) fprintf( stderr, "..." );
			else if( fn->arg_count_min > 0 ) fprintf( stderr, "\b\b" );
			fprintf( stderr, ")\n" );
		}
	}
}
