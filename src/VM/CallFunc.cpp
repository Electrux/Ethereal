/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "ExecInternal.hpp"
#include "CallFunc.hpp"

int CallFunc( vm_state_t & vm, const int ins_ctr )
{
	src_t & src = * vm.srcstack.back();
	instr_t & ins = src.bcode[ ins_ctr ];
	int args_count;
	std::string fn_name;
	std::vector< std::string > arg_types;
	std::vector< var_base_t * > args;
	const function_t * fn;
	modfnptr_t mfnptr = nullptr;
	//langfnptr_t lfnptr = nullptr;
	// 1 for function name
	int func_name_count = 1;
	res_t< var_base_t * > res;
	std::vector< void * > rem_locs;

	VERIFY_STACK_MIN( 1 );
	args_count = std::stoi( ins.oper.val );
	VERIFY_STACK_MIN( ( size_t )args_count + func_name_count );
	if( vm.stack->back()->type() != VT_STR ) {
		VM_FAIL( "expected a name (identifier) for function call, found '%s'",
			 vm.stack->back()->to_str().c_str() );
		goto fail;
	}
	fn_name = vm.stack->back()->to_str();
	vm.stack->pop_back();
	// fetch args
	for( int i = 0; i < args_count; ++i ) {
		arg_types.push_back( vm.stack->back()->type_str() );
		args.push_back( vm.stack->back() );
		vm.stack->pop_back( false );
	}

	// fetch the function pointer from Functions
	fn = vm.funcs.get( fn_name, args_count, arg_types );
	if( fn == nullptr ) {
		VM_FAIL( "function with name '%s' and arg count %d does not exist",
			 fn_name.c_str(), args_count );
		goto fail;
	}
	if( fn->type == FnType::MODULE ) mfnptr = fn->func.modfn;
	//else lfnptr = fn->func.langfn;
	if( mfnptr == nullptr /* && lfnptr == nullptr */ ) {
		VM_FAIL( "function with name '%s' is null", fn_name.c_str() );
		goto fail;
	}

	src.vars.add_scope();

	// execute the function
	res.code = E_OK;
	res.data = mfnptr( args );
	// for lang function, args are moved to vars' new layer which is popped at the end
	for( auto & arg : args ) VAR_DREF( arg );

	if( res.data != nullptr ) {
		vm.stack->push_back( res.data, false );
	}

	src.vars.pop_scope( & rem_locs );
	for( auto & rll : rem_locs ) VAR_DREF( rll );
	return E_OK;
fail:
	// for lang function, args are moved to vars' new layer which is popped at the end
	for( auto & arg : args ) VAR_DREF( arg );
	for( auto & rll : rem_locs ) VAR_DREF( rll );
	return E_VM_FAIL;
}