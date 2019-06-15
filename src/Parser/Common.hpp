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
#include "../Ethereal.hpp"

enum GrammarTypes
{
	GRAM_INVALID = -1,
	GRAM_ENUM,

	_GRAM_LAST,
};

extern const char * GrammarTypeStrs[ _GRAM_LAST ];

class stmt_base_t
{
public:
	const GrammarTypes m_type;
	const int m_line;
	const int m_col;
	stmt_base_t( const GrammarTypes type, const int line, const int col );
	virtual ~stmt_base_t();
	virtual void disp( const bool has_next ) const = 0;
};

class stmt_enum_t : public stmt_base_t
{
	const tok_t * m_name;
	const std::vector< tok_t * > m_vals;
public:
	stmt_enum_t( const tok_t * name, const std::vector< tok_t * > & vals,
		     const int line, const int col );
	~stmt_enum_t();
	void disp( const bool has_next ) const;
};



class parse_helper_t
{
	toks_t & m_toks;
	int m_token_ctr;
	tok_t m_invalid;
public:
	parse_helper_t( toks_t & toks );

	tok_t * peak( const int num = 0 );
	tok_t * next();
	tok_t * prev();

	bool has_next() const;

	int tok_ctr() const;
};

#define PARSE_FAIL( ... ) src_fail( src.code[ ph.peak()->line - 1 ], \
				  ph.peak()->line, ph.peak()->col + 1, __VA_ARGS__ )

#endif // PARSER_COMMON_HPP