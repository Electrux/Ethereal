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
		   m_stype == SIMPLE_TOKEN ? m_val->data.c_str() : TokStrs[ m_val->type ],
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
	"Struct",
	"Function",
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