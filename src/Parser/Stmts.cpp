/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Stmts.hpp"

const char * GrammarTypeStrs[ _GRAM_LAST ] = {
	"Simple",
	"Expression",
	"Enum",
	"Ldmod",
	"Import",
	"Struct",
	"Block",
	"Function Def",
	"Func/Struct Call",
	"If Conditional",
	"For Loop",
	"Return",
	"Continue",
	"Break",
};

stmt_base_t::stmt_base_t( const GrammarTypes type, const int tok_ctr )
	: m_type( type ), m_tok_ctr( tok_ctr ) {}
stmt_base_t::~stmt_base_t() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// Simple /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

const char * SimpleTypeStrs[ _SIMPLE_LAST ] = {
	"Token",
	"Operator",
	"Keyword",
};

stmt_simple_t::stmt_simple_t( const SimpleType stype, const tok_t * val,
		       	      const int tok_ctr )
	: stmt_base_t( GRAM_SIMPLE, tok_ctr ), m_stype( stype ), m_val( val ) {}
stmt_simple_t::~stmt_simple_t() {}
void stmt_simple_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Simple at %x\n", this );
	IO::tab_add( false );
	IO::print( false, "Name: %s (type: %s)\n",
		   m_stype == SIMPLE_TOKEN && !m_val->data.empty() ? m_val->data.c_str() : TokStrs[ m_val->type ],
		   SimpleTypeStrs[ m_stype ] );
	IO::tab_rem( 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Enum //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_enum_t::stmt_enum_t( const tok_t * name, const std::vector< tok_t * > & vals,
			  const int tok_ctr )
	: stmt_base_t( GRAM_ENUM, tok_ctr ), m_name( name ), m_vals( vals ) {}
stmt_enum_t::~stmt_enum_t() {}

void stmt_enum_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Enum at: %x\n", this );

	IO::tab_add( false );
	IO::print( true, "Name: %s\n", m_name->data.c_str() );
	IO::print( false, "Values:\n" );
	IO::tab_add( false );
	for( size_t i = 0; i < m_vals.size(); ++i ) {
		IO::print( i != m_vals.size() - 1, "%s\n", m_vals[ i ]->data.c_str() );
	}
	IO::tab_rem( 3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// ldmod /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_ldmod_t::stmt_ldmod_t( const tok_t * what, const tok_t * as,
			    const int tok_ctr )
	: stmt_base_t( GRAM_LDMOD, tok_ctr ), m_what( what ), m_as( as ) {}
stmt_ldmod_t::~stmt_ldmod_t() {}

void stmt_ldmod_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Load Module at: %x\n", this );

	IO::tab_add( false );
	IO::print( true, "Name: %s\n", m_what->data.c_str() );
	IO::print( false, "As: %s\n", m_as == nullptr ? "(none)" : m_as->data.c_str() );
	IO::tab_rem( 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// import /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_import_t::stmt_import_t( const std::vector< tok_t * > & what, const tok_t * as,
			      const int tok_ctr )
	: stmt_base_t( GRAM_IMPORT, tok_ctr ), m_what( what ), m_as( as ) {}
stmt_import_t::~stmt_import_t() {}

void stmt_import_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Import at: %x\n", this );

	IO::tab_add( false );
	IO::print( true, "Name: " );
	for( auto & what : m_what ) {
		fprintf( stdout, "%s.", what->data.c_str() );
	}
	fprintf( stdout, "\b \b\n" );
	IO::print( false, "As: %s\n", m_as == nullptr ? "(none)" : m_as->data.c_str() );
	IO::tab_rem( 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Expr //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

const char * ExprTypeStrs[ _EXPR_LAST ] = {
	"Basic",
	"Array",
	"Map",
};

stmt_expr_t::stmt_expr_t( const ExprType etype, const stmt_base_t * lhs, const stmt_simple_t * oper,
			  const stmt_base_t * rhs, const int tok_ctr )
	: stmt_base_t( GRAM_EXPR, tok_ctr ), m_lhs( lhs ),
	  m_rhs( rhs ), m_oper( oper ), m_etype( etype ), m_is_top_expr( false ),
	  m_annotation( nullptr ) {}
stmt_expr_t::~stmt_expr_t()
{
	if( m_lhs ) delete m_lhs;
	if( m_rhs ) delete m_rhs;
	if( m_oper ) delete m_oper;
	if( m_annotation ) delete m_annotation;
}

void stmt_expr_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	if( m_annotation ) {
		IO::print( has_next, "Expression (top: %s) (type: %s) (name: %s) at: %x\n",
			   m_is_top_expr ? "yes" : "no", ExprTypeStrs[ m_etype ],
			   m_annotation->m_val->data.c_str(), this );
	} else {
		IO::print( has_next, "Expression (top: %s) (type: %s) at: %x\n",
			   m_is_top_expr ? "yes" : "no", ExprTypeStrs[ m_etype ], this );
	}

	IO::tab_add( m_lhs != nullptr || m_rhs != nullptr );
	IO::print( m_lhs != nullptr || m_rhs != nullptr,
		   "Operator: %s\n", m_oper != nullptr ? TokStrs[ m_oper->m_val->type ] : "(null)" );
	if( m_lhs != nullptr ) {
		IO::print( m_rhs != nullptr, "LHS:\n" );
		m_lhs->disp( false );
	}
	IO::tab_rem();
	IO::tab_add( false );
	if( m_rhs != nullptr ) {
		IO::print( false, "RHS:\n" );
		m_rhs->disp( false );
	}

	IO::tab_rem( 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// Struct /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_struct_t::stmt_struct_t( const stmt_simple_t * name, const std::vector< struct_field_t > & fields, const int tok_ctr )
	: stmt_base_t( GRAM_STRUCT, tok_ctr ), m_name( name ), m_fields( fields ) {}
stmt_struct_t::~stmt_struct_t()
{
	delete m_name;
	for( auto & f : m_fields ) {
		if( f.name ) delete f.name;
		if( f.def_val ) delete f.def_val;
	}
}

void stmt_struct_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Struct %s at: %x\n", m_name->m_val->data.c_str(), this );

	for( size_t i = 0; i < m_fields.size(); ++i ) {
		IO::tab_add( i != m_fields.size() - 1 );
		IO::print( i != m_fields.size() - 1, "Field %d:\n", i + 1 );
		m_fields[ i ].name->disp( true );
		m_fields[ i ].def_val->disp( false );
		IO::tab_rem();
	}
	IO::tab_rem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Block /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_block_t::stmt_block_t( std::vector< stmt_base_t * > * stmts, const int tok_ctr )
	: stmt_base_t( GRAM_BLOCK, tok_ctr ), m_stmts( stmts ) {}

stmt_block_t::~stmt_block_t()
{
	for( auto & stmt : * m_stmts ) delete stmt;
	delete m_stmts;
}

void stmt_block_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Block at: %x\n", this );

	for( size_t i = 0; i < m_stmts->size(); ++i ) {
		( * m_stmts )[ i ]->disp( i != m_stmts->size() - 1 );
	}
	IO::tab_rem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Function ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_func_t::stmt_func_t( const stmt_simple_t * name, const stmt_expr_t * args,
			  const stmt_block_t * block, const stmt_expr_t * mem_type,
			  const int tok_ctr )
	: stmt_base_t( GRAM_FUNC, tok_ctr ), m_name( name ), m_args( args ), m_block( block ),
	  m_mem_type( mem_type ) {}

stmt_func_t::~stmt_func_t()
{
	delete m_name;
	if( m_args ) delete m_args;
	delete m_block;
	if( m_mem_type ) delete m_mem_type;
}

void stmt_func_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Function %s at: %x\n", m_name->m_val->data.c_str(), this );

	if( m_mem_type ) {
		IO::tab_add( true );
		IO::print( true, "Member type:\n" );
		m_mem_type->disp( false );
		IO::tab_rem();
	}

	if( m_args ) {
		IO::tab_add( true );
		IO::print( true, "Arguments:\n" );
		m_args->disp( false );
		IO::tab_rem();
	}

	m_block->disp( false );

	IO::tab_rem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Function/Struct Call //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_func_struct_call_t::stmt_func_struct_call_t( const stmt_simple_t * name,
						  const stmt_expr_t * args,
						  const int tok_ctr )
	: stmt_base_t( GRAM_FN_STRUCT_CALL, tok_ctr ),
	  m_name( name ), m_args( args ), m_is_struct( false ) {}

stmt_func_struct_call_t::~stmt_func_struct_call_t()
{
	delete m_name;
	if( m_args ) delete m_args;
}
void stmt_func_struct_call_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	if( m_is_struct ) {
		IO::print( has_next, "Struct '%s' instantiation at: %x\n", m_name->m_val->data.c_str(), this );
	} else {
		IO::print( has_next, "Function '%s' call at: %x\n", m_name->m_val->data.c_str(), this );
	}
	if( m_args ) {
		IO::tab_add( false );
		IO::print( false, "Arguments:\n" );
		m_args->disp( false );
		IO::tab_rem();
	}
	IO::tab_rem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Conditional //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_if_t::stmt_if_t( const std::vector< condition_t > & conds, const int tok_ctr )
	: stmt_base_t( GRAM_IF, tok_ctr ), m_conds( conds ) {}

stmt_if_t::~stmt_if_t()
{
	for( auto & c : m_conds ) {
		if( c.cond ) delete c.cond;
		delete c.block;
	}
}

void stmt_if_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Conditional at: %x\n", this );

	for( size_t i = 0; i < m_conds.size(); ++i ) {
		IO::tab_add( i != m_conds.size() - 1 );
		IO::print( i != m_conds.size() - 1, "Case: %d\n", i + 1 );
		if( m_conds[ i ].cond != nullptr ) {
			m_conds[ i ].cond->disp( true );
		}
		m_conds[ i ].block->disp( false );
		IO::tab_rem( 1 );
	}

	IO::tab_rem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Loop //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_for_t::stmt_for_t( stmt_expr_t * init, stmt_expr_t * cond, stmt_expr_t * step,
		     const stmt_block_t * block, const int tok_ctr )
	: stmt_base_t( GRAM_FOR, tok_ctr ), m_init( init ), m_cond( cond ), m_step( step ),
	  m_block( block ) {}

stmt_for_t::~stmt_for_t()
{
	if( m_init != nullptr ) delete m_init;
	if( m_cond != nullptr ) delete m_cond;
	if( m_step != nullptr ) delete m_step;
	if( m_block != nullptr ) delete m_block;
}

void stmt_for_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Loop at: %x\n", this );

	IO::tab_add( true );
	IO::print( true, "Expression (init):\n" );
	if( m_init != nullptr ) m_init->disp( false );
	IO::print( true, "Expression (cond):\n" );
	if( m_cond != nullptr ) m_cond->disp( false );
	IO::print( true, "Expression (step):\n" );
	if( m_step != nullptr ) m_step->disp( false );
	IO::tab_rem();
	IO::tab_add( false );
	IO::print( false, "Block:\n" );
	if( m_block != nullptr ) m_block->disp( false );

	IO::tab_rem( 2 );
}


////////////////////////////////////////////
/////////////// stmt_return_t //////////////
////////////////////////////////////////////

stmt_return_t::stmt_return_t( stmt_expr_t * ret_val, const int tok_ctr )
	: stmt_base_t( GRAM_RETURN, tok_ctr ), m_ret_val( ret_val ) {}
stmt_return_t::~stmt_return_t() { if( m_ret_val != nullptr ) delete m_ret_val; }

void stmt_return_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Return at: %x\n", this );

	m_ret_val->disp( false );

	IO::tab_rem( 1 );
}

////////////////////////////////////////////
/////////////// stmt_continue_t //////////////
////////////////////////////////////////////

stmt_continue_t::stmt_continue_t( const int tok_ctr )
	: stmt_base_t( GRAM_CONTINUE, tok_ctr ) {}
stmt_continue_t::~stmt_continue_t() {}

void stmt_continue_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, " Continue statement at: %x\n", this );
	IO::tab_rem();
}

////////////////////////////////////////////
/////////////// stmt_break_t //////////////
////////////////////////////////////////////

stmt_break_t::stmt_break_t( const int tok_ctr )
	: stmt_base_t( GRAM_BREAK, tok_ctr ) {}
stmt_break_t::~stmt_break_t() {}

void stmt_break_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, " Break statement at: %x\n", this );
	IO::tab_rem();
}
