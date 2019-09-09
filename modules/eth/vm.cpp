/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../src/VM/Core.hpp"

#include "../../src/FE/CmdArgs.hpp"
#include "../../src/VM/ExecInternal.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Class ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_evm_t : public var_base_t
{
	vm_state_t * m_vm;
	bool m_copied;
public:
	// can't use pointer for m_pre_ph because it doesn't get freed correctly
	// since it uses reference to m_vm->srcstack.back()
	parse_helper_t m_pre_ph;
	var_evm_t( vm_state_t * vm, const int parse_ctr = 0 );
	~var_evm_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	vm_state_t * & get();
};
#define AS_EVM( x ) static_cast< var_evm_t * >( x )

var_evm_t::var_evm_t( vm_state_t * vm, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, parse_ctr ), m_vm( vm ),
	  m_copied( false ), m_pre_ph( vm->srcstack.back()->toks )
{}
var_evm_t::~var_evm_t()
{
	if( !m_copied ) {
		m_vm->srcstack.pop_back();
		delete m_vm;
	}
}

std::string var_evm_t::type_str() const { return "evm_t"; }
std::string var_evm_t::to_str() const
{
	std::string str = "evm_t{";
	str += m_vm->srcstack.back()->name + "}";
	return str;
}
mpz_class var_evm_t::to_int() const { return m_vm->exit_status; }
bool var_evm_t::to_bool() const { return m_vm->exit_called; }
var_base_t * var_evm_t::copy( const int parse_ctr )
{
	m_copied = true;
	return new var_evm_t( m_vm, parse_ctr );
}
void var_evm_t::assn( var_base_t * b )
{
	var_evm_t * tmp = AS_EVM( b );
	if( !m_copied ) {
		delete tmp->m_vm;
	}

	m_vm = tmp->m_vm;
	tmp->m_copied = true;
	m_copied = false;
}
vm_state_t * & var_evm_t::get() { return m_vm; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * vm_create( vm_state_t & vm, func_call_data_t & fcd )
{
	int err = E_OK;

	src_t * src = new src_t( true );
	src->ptree = new parse_tree_t;
	src->dir = vm.srcstack.back()->dir;
	src->name = fcd.args.size() > 1 ? fcd.args[ 0 ]->to_str() : "<repl>";
	src->id = src->name;

	vm_state_t * v = new vm_state_t();

	v->flags = vm.flags;
	v->srcstack.push_back( src );
	v->srcs[ src->id ] = src;

	var_base_t * prog = vm.vars->get( "__PROG__" );
	var_base_t * args = vm.vars->get( "args" );
	var_base_t * tru = vm.vars->get( "true" );
	var_base_t * fal = vm.vars->get( "false" );

	if( !set_init_mods( * v ) ) { err = E_VM_FAIL; goto fail; }

	VAR_IREF( prog );
	VAR_IREF( args );
	VAR_IREF( tru );
	VAR_IREF( fal );

	v->vars->add( "__PROG__", prog );
	v->vars->add( "args", args );
	v->vars->add( "true", tru );
	v->vars->add( "false", fal );

	VAR_IREF( v->nil );
	v->vars->add( "nil", v->nil );

	return new var_evm_t( v );
fail:
	delete v;
	return vm.nil;
}

var_base_t * vm_is_running( vm_state_t & vm, func_call_data_t & fcd )
{
	var_evm_t * v = AS_EVM( fcd.args[ 0 ] );
	return TRUE_FALSE( !v->get()->exit_called );
}

var_base_t * vm_exec_code( vm_state_t & vm, func_call_data_t & fcd )
{
	std::vector< std::string > new_code;
	std::vector< var_base_t * > vec = AS_VEC( fcd.args[ 1 ] )->get();
	vm_state_t * v = AS_EVM( fcd.args[ 0 ] )->get();
	src_t * s = v->srcstack.back();
	src_t src( false );
	src.name = s->name;
	src.id = s->name;

	size_t code_size = s->code.size();
	size_t toks_size = s->toks.size();
	size_t ptree_size = s->ptree->size();
	size_t bcode_size = s->bcode.size();

	// add old code and bytecode entries for correct line/bytecode numbers of new ones
	for( size_t i = 0; i < code_size; ++i ) src.code.push_back( "" );
	for( size_t i = 0; i < bcode_size; ++i ) src.bcode.push_back( {} );

	// add the new code
	for( auto & e : vec ) {
		src.code.push_back( e->to_str() );
	}

	int err = E_OK;

	err = tokenize( src );
	if( err != E_OK ) return new var_int_t( err );
	if( v->flags & OPT_T ) {
		fprintf( stdout, "Tokens:\n" );
		auto & toks = src.toks;
		for( size_t i = 0; i < toks.size(); ++i ) {
			auto & tok = toks[ i ];
			fprintf( stdout, "ID: %zu\tType: %s\tLine: %d[%d]\tSymbol: %s\n",
				i, TokStrs[ tok.type ], tok.line, tok.col, tok.data.c_str() );
		}
	}
	src.ptree = parse( src );
	if( src.ptree == nullptr ) return new var_int_t( E_PARSE_FAIL );

	if( v->flags & OPT_P ) {
		fprintf( stdout, "Parse Tree:\n" );
		for( auto it = src.ptree->begin(); it != src.ptree->end(); ++it ) {
			( * it )->disp( it != src.ptree->end() - 1 );
		}
	}

	for( auto & it : * src.ptree ) {
		if( !it->bytecode( src ) ) return new var_int_t( E_BYTECODE_FAIL );
	}

	if( v->flags & OPT_B ) {
		fprintf( stdout, "Byte Code:\n" );
		for( size_t i = 0; i < src.bcode.size(); ++i ) {
			auto & ins = src.bcode[ i ];
			fprintf( stdout, "%-*zu %-*s%-*s[%s]\n",
				 5, i, 20, InstrCodeStrs[ ins.opcode ], 7, OperTypeStrs[ ins.oper.type ], ins.oper.val.c_str() );
		}
	}

	s->code.insert( s->code.end(), src.code.begin() + code_size, src.code.end() );
	s->toks.insert( s->toks.end(), src.toks.begin(), src.toks.end() );
	s->ptree->insert( s->ptree->end(), src.ptree->begin(), src.ptree->end() );
	s->bcode.insert( s->bcode.end(), src.bcode.begin() + bcode_size, src.bcode.end() );

	v->bcodectr.push_back( bcode_size );
	int res = exec_internal( * v, bcode_size );
	v->bcodectr.pop_back();
	if( res != E_OK ) {
		s->code.erase( s->code.begin() + code_size, s->code.end() );
		s->toks.erase( s->toks.begin() + toks_size, s->toks.end() );
		s->ptree->erase( s->ptree->begin() + ptree_size, s->ptree->end() );
		s->bcode.erase( s->bcode.begin() + bcode_size, s->bcode.end() );
	} else {
		src.ptree->clear();
	}
	AS_INT( fcd.args[ 2 ] )->get() = v->exit_status;
	return new var_int_t( res );
}

REGISTER_MODULE( vm )
{
	functions_t & vmfns = vm.typefuncs[ "_evm" ];
	vmfns.add( { "new", 0, 0, {}, FnType::MODULE, { .modfn = vm_create }, true } );

	functions_t & evmfns = vm.typefuncs[ "evm_t" ];
	evmfns.add( { "is_running", 0, 0, {}, FnType::MODULE, { .modfn = vm_is_running }, false } );
	evmfns.add( { "exec_code", 2, 2, { "vec", "int" }, FnType::MODULE, { .modfn = vm_exec_code }, true } );
}