/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

#include "IO.hpp"
#include "ParseHelper.hpp"

enum GrammarTypes
{
	GRAM_INVALID = -1,
	GRAM_SIMPLE,
	GRAM_EXPR,
	GRAM_ENUM,
	GRAM_LDMOD,
	GRAM_IMPORT,
	GRAM_STRUCT,
	GRAM_BLOCK,
	GRAM_FUNC,
	GRAM_FN_STRUCT_CALL,
	GRAM_IF,
	GRAM_FOR,
	GRAM_RETURN,
	GRAM_CONTINUE,
	GRAM_BREAK,

	_GRAM_LAST,
};

extern const char * GrammarTypeStrs[ _GRAM_LAST ];

class stmt_base_t
{
public:
	const GrammarTypes m_type;
	const int m_tok_ctr;
	stmt_base_t( const GrammarTypes type, const int tok_ctr );
	virtual ~stmt_base_t();
	virtual void disp( const bool has_next ) const = 0;
};

enum SimpleType
{
	SIMPLE_TOKEN,
	SIMPLE_OPER,
	SIMPLE_KEYWORD,

	_SIMPLE_LAST,
};

extern const char * SimpleTypeStrs[ _SIMPLE_LAST ];

class stmt_simple_t : public stmt_base_t
{
public:
	const SimpleType m_stype;
	const tok_t * m_val;
	stmt_simple_t( const SimpleType stype, const tok_t * val,
		       const int tok_ctr );
	~stmt_simple_t();
	void disp( const bool has_next ) const;
};

class stmt_enum_t : public stmt_base_t
{
	const tok_t * m_name;
	const std::vector< tok_t * > m_vals;
public:
	stmt_enum_t( const tok_t * name, const std::vector< tok_t * > & vals,
		     const int tok_ctr );
	~stmt_enum_t();
	void disp( const bool has_next ) const;
};

class stmt_ldmod_t : public stmt_base_t
{
	const tok_t * m_what;
	const tok_t * m_as;
public:
	stmt_ldmod_t( const tok_t * what, const tok_t * as,
		      const int tok_ctr );
	~stmt_ldmod_t();
	void disp( const bool has_next ) const;
};

class stmt_import_t : public stmt_base_t
{
	const std::vector< tok_t * > m_what;
	const tok_t * m_as;
public:
	stmt_import_t( const std::vector< tok_t * > & what, const tok_t * as,
		       const int tok_ctr );
	~stmt_import_t();
	void disp( const bool has_next ) const;
};

enum ExprType
{
	EXPR_BASIC,
	EXPR_ARRAY,
	EXPR_MAP,

	_EXPR_LAST,
};

extern const char * ExprTypeStrs[ _EXPR_LAST ];

class stmt_expr_t : public stmt_base_t
{
	const stmt_base_t * m_lhs, * m_rhs;
	const stmt_simple_t * m_oper;
public:
	ExprType m_etype;
	bool m_is_top_expr;
	const stmt_simple_t * m_annotation;
	stmt_expr_t( const ExprType etype, const stmt_base_t * lhs, const stmt_simple_t * oper,
		     const stmt_base_t * rhs, const int tok_ctr );
	~stmt_expr_t();
	void disp( const bool has_next ) const;
};

struct struct_field_t
{
	const stmt_simple_t * name;
	stmt_expr_t * def_val;
};

class stmt_struct_t : public stmt_base_t
{
	const stmt_simple_t * m_name;
	const std::vector< struct_field_t > m_fields;
public:
	stmt_struct_t( const stmt_simple_t * name, const std::vector< struct_field_t > & fields,
		       const int tok_ctr );
	~stmt_struct_t();
	void disp( const bool has_next ) const;
};

class stmt_block_t : public stmt_base_t
{
	std::vector< stmt_base_t * > * m_stmts;
public:
	stmt_block_t( std::vector< stmt_base_t * > * stmts, const int tok_ctr );
	~stmt_block_t();
	void disp( const bool has_next ) const;
};

class stmt_func_t : public stmt_base_t
{
	const stmt_simple_t * m_name;
	const stmt_expr_t * m_args;
	const stmt_block_t * m_block;
	const stmt_expr_t * m_mem_type;
public:
	stmt_func_t( const stmt_simple_t * name, const stmt_expr_t * args,
		     const stmt_block_t * block, const stmt_expr_t * mem_type,
		     const int tok_ctr );
	~stmt_func_t();
	void disp( const bool has_next ) const;
};

class stmt_func_struct_call_t : public stmt_base_t
{
	const stmt_simple_t * m_name;
	const stmt_expr_t * m_args;
public:
	bool m_is_struct;
	stmt_func_struct_call_t( const stmt_simple_t * name, const stmt_expr_t * args,
				 const int tok_ctr );
	~stmt_func_struct_call_t();
	void disp( const bool has_next ) const;
};

struct condition_t
{
	const stmt_expr_t * cond;
	const stmt_block_t * block;
};

class stmt_if_t : public stmt_base_t
{
	std::vector< condition_t > m_conds;
public:
	stmt_if_t( const std::vector< condition_t > & conds, const int tok_ctr );
	~stmt_if_t();
	void disp( const bool has_next ) const;
};

class stmt_for_t : public stmt_base_t
{
	stmt_expr_t * m_init, * m_cond, * m_step;
	const stmt_block_t * m_block;
public:
	stmt_for_t( stmt_expr_t * init, stmt_expr_t * cond, stmt_expr_t * step,
		     const stmt_block_t * block, const int tok_ctr );
	~stmt_for_t();
	void disp( const bool has_next ) const;
};

struct stmt_return_t : public stmt_base_t
{
	stmt_expr_t * m_ret_val;
	stmt_return_t( stmt_expr_t * ret_val, const int tok_ctr );
	~stmt_return_t();
	void disp( const bool has_next ) const;
};

struct stmt_continue_t : public stmt_base_t
{
	stmt_continue_t( const int tok_ctr );
	~stmt_continue_t();
	void disp( const bool has_next ) const;
};

struct stmt_break_t : public stmt_base_t
{
	stmt_break_t( const int tok_ctr );
	~stmt_break_t();
	void disp( const bool has_next ) const;
};

#define PARSE_FAIL( ... ) src_fail( src.code[ ph->peak()->line - 1 ], \
				    ph->peak()->line, ph->peak()->col, __VA_ARGS__ )

#endif // PARSER_COMMON_HPP