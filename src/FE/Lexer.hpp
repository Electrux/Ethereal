/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>

#include "Core.hpp"

enum TokType
{
	TOK_INT,
	TOK_FLT,

	TOK_STR,
	TOK_IDEN,

	//Keywords
	TOK_TRUE,
	TOK_FALSE,
	TOK_ENUM,
	TOK_IMPORT,
	TOK_AS,
	TOK_NIL,
	TOK_STRUCT,
	TOK_FN,
	TOK_MFN,
	TOK_RETURN,
	TOK_IF,
	TOK_ELIF,
	TOK_ELSE,
	TOK_FOR,
	TOK_CONTINUE,
	TOK_BREAK,
	TOK_LDMOD,
	// if the source file is the actual executed one, execute the code between these symbols
	TOK_MAIN_SRC_BEG,
	TOK_MAIN_SRC_END,

	// Operators
	TOK_ASSN,
	// Arithmetic
	TOK_ADD,
	TOK_SUB,
	TOK_MUL,
	TOK_DIV,
	TOK_MOD,
	TOK_ADD_ASSN,
	TOK_SUB_ASSN,
	TOK_MUL_ASSN,
	TOK_DIV_ASSN,
	TOK_MOD_ASSN,
	TOK_POW, // **
	// Unary
	TOK_UADD,
	TOK_USUB,
	// Logic
	TOK_AND,
	TOK_OR,
	TOK_NOT,
	TOK_EQ,
	TOK_LT,
	TOK_GT,
	TOK_LE,
	TOK_GE,
	TOK_NE,
	// Bitwise
	TOK_BAND,
	TOK_BOR,
	TOK_BNOT,
	TOK_BXOR,
	TOK_BAND_ASSN,
	TOK_BOR_ASSN,
	TOK_BNOT_ASSN,
	TOK_BXOR_ASSN,
	// Others
	TOK_LSHIFT,
	TOK_RSHIFT,
	TOK_LSHIFT_ASSN,
	TOK_RSHIFT_ASSN,
	// Conditional ( ? : )
	TOK_QUEST,
	TOK_COL,  // :

	// Separators
	TOK_DOT,
	TOK_COMMA,
	TOK_AT,
	TOK_SPC,
	TOK_TAB,
	TOK_NEWL,
	TOK_COLS, // Semi colon
	// Parenthesis, Braces, Brackets
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LBRACK,
	TOK_RBRACK,

	TOK_INVALID,

	_TOK_LAST,
};

extern const char * TokStrs[ _TOK_LAST ];

struct tok_t
{
	int line;
	int col;
	TokType type;
	std::string data;

	tok_t( int _line, int _col, int _type, std::string _data ) :
		line( _line ), col( _col ), type( ( TokType)_type ), data( _data ) {}
};

typedef std::vector< tok_t > toks_t;

struct src_t;

int tokenize( src_t & src );

inline bool token_type_is_data( const int type )
{
	return type == TOK_INT || type == TOK_FLT || type == TOK_STR || type == TOK_IDEN ||
	       type == TOK_TRUE || type == TOK_FALSE || type == TOK_NIL;
}

inline bool token_type_is_oper( const int type )
{
	return ( type >= TOK_ASSN && type <= TOK_RBRACK );
}

inline bool token_type_is_one_of_assign( const int type )
{
	return ( type == TOK_ASSN ||
		 type == TOK_ADD_ASSN ||
		 type == TOK_SUB_ASSN ||
		 type == TOK_MUL_ASSN ||
		 type == TOK_DIV_ASSN ||
		 type == TOK_MOD_ASSN ||
		 type == TOK_BAND_ASSN ||
		 type == TOK_BOR_ASSN ||
		 type == TOK_BNOT_ASSN ||
		 type == TOK_BXOR_ASSN ||
		 type == TOK_LSHIFT_ASSN ||
		 type == TOK_RSHIFT_ASSN
		);
}

inline bool token_is_data( const tok_t * tok )
{
	return token_type_is_data( tok->type );
}

inline bool token_is_oper( const tok_t * tok )
{
	return token_type_is_oper( tok->type );
}

inline bool token_is_one_of_assign( const tok_t * tok )
{
	return token_type_is_one_of_assign( tok->type );
}

#endif // LEXER_HPP