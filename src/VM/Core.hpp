/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_CORE_HPP
#define VM_CORE_HPP

#include "../FE/Ethereal.hpp"
#include "DynLib.hpp"
#include "Consts.hpp"
#include "VMStack.hpp"

typedef std::vector< src_t * > src_stack_t;

typedef std::unordered_map< std::string, functions_t > type_funcs_t;

typedef std::unordered_map< std::string, var_struct_def_t * > structs_t;

#define MAX_BCODE_CTR_SRCS 100

struct vm_state_t
{
	size_t flags;
	bool exit_called;
	int exit_status;

	std::vector< int > bcodectr;

	var_none_t * none;
	var_nil_t * nil;

	src_stack_t srcstack;
	srcs_t srcs;

	vars_t * vars;
	structs_t structs;

	dyn_lib_t * dlib;
	consts_t * consts;
	functions_t funcs;
	vm_stack_t * stack;

	type_funcs_t typefuncs;

	vm_state_t();
	~vm_state_t();
};

// for CallFunc()
struct func_call_data_t
{
	std::string fn_name;
	int args_count;
	std::vector< std::string > arg_types;
	std::vector< var_base_t * > args;
	std::vector< void * > rem_locs;
	int parse_ctr;
};

#define VM_FAIL( ... ) src_fail( src.name, src.code[ ins.line - 1 ], \
				 ins.line, ins.col, __VA_ARGS__ )

#define VM_FAIL_TOK_CTR( tok_ctr, ... ) src_fail( src.name, src.code[ src.toks[ tok_ctr ].line - 1 ],	\
						  src.toks[ tok_ctr ].line, src.toks[ tok_ctr ].col,	\
						  __VA_ARGS__ )

bool set_init_mods( vm_state_t & vm );

size_t mpz_to_size_t( const mpz_class & n );

std::vector< std::string > str_delimit( const std::string & str, const char ch, const bool first_only = false );

#define TRUE_FALSE( condition ) ( condition ) ? vm.vars->get( "true" ) : vm.vars->get( "false" )

#endif // VM_CORE_HPP