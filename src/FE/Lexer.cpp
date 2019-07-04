/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cctype>
#include <stdarg.h>

#include "FS.hpp"
#include "Lexer.hpp"

const char * TokStrs[ _TOK_LAST ] = {
	"INT",
	"FLT",

	"STR",
	"IDEN",

	//Keywords
	"true",
	"false",
	"enum",
	"import",
	"as",
	"nil",
	"struct",
	"fn",
	"mfn",
	"return",
	"if",
	"elif",
	"else",
	"for",
	"continue",
	"break",
	"ldmod",
	"self", // using the struct reference for member functions
	// if the source file is the actual executed one, execute the code between these symbols
	"_MAIN_BEG_",
	"_MAIN_END_",

	// Operators
	"=",
	// Arithmetic
	"+",
	"-",
	"*",
	"/",
	"%",
	"+=",
	"-=",
	"*=",
	"/=",
	"%=",
	"**", // power
	// Unary (used by parser (in Expression.cpp))
	"u+",
	"u-",
	// Logic
	"&&",
	"||",
	"!",
	"==",
	"<",
	">",
	"<=",
	">=",
	"!=",
	// Bitwise
	"&",
	"|",
	"~",
	"^",
	"&=",
	"|=",
	"~=",
	"^=",
	// Others
	"<<",
	">>",
	"<<=",
	">>=",
	// Conditional ( ? : )
	"?",
	":",

	// Separators
	".",
	",",
	"@",
	"SPC",
	"TAB",
	"NEWL",
	";",
	// Parenthesis, Braces, Brackets
	"(",
	")",
	"{",
	"}",
	"[",
	"]",

	"<EOF>",
};

#define CURR( line ) ( line[ i ] )
#define NEXT( line ) ( i + 1 < ( int )line.size() ? line[ i + 1 ] : 0 )
#define PREV( line ) ( line.size() > 0 && i - 1 >= 0 ? line[ i - 1 ] : 0 )
#define SET_OP_TYPE_BRK( type ) op_type = type; break

#define SRC_FAIL( ... ) src_fail( line, line_num, i + 1, __VA_ARGS__ )

static int tokenize_line( const std::string & line, const int line_len, const int line_num,
			  toks_t & toks, const bool is_main_src );

static std::string get_name( const std::string & input, const int input_len, int & i );
static int classify_str( const std::string & str );
static std::string get_num( const std::string & input, const int input_len, const int line_num, int & i, int & num_type );
static int get_const_str( const std::string & input, const int input_len, const int line_num, int & i, std::string & buf );
static int get_operator( const std::string & input, const int input_len, const int line_num, int & i );
static inline bool is_valid_num_char( const char c );

// TODO: The src stack and map shall be updated prior to this function
int tokenize( src_t & src )
{
	int res = read_file( src );
	if( res == E_FILE_IO_ERR ) {
		return res;
	}
	if( res == E_FILE_EMPTY ) {
		fprintf( stderr, "error: invalid source (empty file): %s\n", src.name.c_str() );
		return res;
	}
	const auto & lines = src.code;
	const int line_count = lines.size();
	auto & toks = src.toks;

	// tokenize the input
	for( int i = 0; i < line_count; ++i ) {
		auto & line = lines[ i ];
		const int line_len = line.size();
		if( line_len < 1 || line[ 0 ] == '\n' ) continue;
		int res = tokenize_line( line, line_len, i + 1, toks, src.is_main_src );
		if( res != E_OK ) return res;
	}

	return E_OK;
}

static int tokenize_line( const std::string & line, const int line_len, const int line_num,
			  toks_t & toks, const bool is_main_src )
{
	int i = 0;
	int err = E_OK;

	static bool comment_block = false;
	static bool main_src_block = false;

	while( i < line_len ) {
		if( isspace( line[ i ] ) ){ ++i; continue; }

		if( CURR( line ) == '*' && NEXT( line ) == '/' ) {
			if( !comment_block ) {
				SRC_FAIL( "encountered multi line comment terminator '*/' "
					  "in non commented block", line_num, i + 1 );
				err = E_LEX_FAIL;
				break;
			}
			i += 2;
			comment_block = false;
			continue;
		}

		if( comment_block ) { ++i; continue; }

		if( CURR( line ) == '/' && NEXT( line ) == '*' ) {
			i += 2;
			comment_block = true;
			continue;
		}

		if( CURR( line ) == '#' ) break;

		// for storing correct columns in tokens (+1 for zero indexing)
		int tmp_i = i + 1;
		// strings
		if( isalpha( CURR( line ) ) || CURR( line ) == '_' ) {
			std::string str = get_name( line, line_len, i );
			if( str == TokStrs[ TOK_MAIN_SRC_BEG ] ) {
				if( !is_main_src ) main_src_block = true;
				continue;
			} else if( str == TokStrs[ TOK_MAIN_SRC_END ] ) {
				if( !is_main_src ) main_src_block = false;
				continue;
			}
			if( main_src_block ) continue;
			// check if string is a keyword
			int kw_or_iden = classify_str( str );
			toks.emplace_back( line_num, tmp_i, kw_or_iden, str );
			continue;
		}

		if( main_src_block ) { ++i; continue; }

		// numbers
		if( isdigit( CURR( line ) ) ) {
			int num_type = TOK_INT;
			std::string num = get_num( line, line_len, line_num, i, num_type );
			if( num.empty() ) {
				err = E_PARSE_FAIL;
				break;
			}
			toks.emplace_back( line_num, tmp_i, num_type, num );
			continue;
		}

		// const strings
		if( CURR( line ) == '\"' || CURR( line ) == '\'' ) {
			std::string str;
			int res = get_const_str( line, line_len, line_num, i, str );
			if( res != E_OK ) {
				err = res;
				break;
			}
			toks.emplace_back( line_num, tmp_i, TOK_STR, str );
			continue;
		}

		// operators
		int op_type = get_operator( line, line_len, line_num, i );
		if( op_type < 0 ) {
			err = E_LEX_FAIL;
			break;
		}
		toks.emplace_back( line_num, tmp_i, op_type, "" );
	}

	return err;
}

static std::string get_name( const std::string & line, const int line_len, int & i )
{
	std::string buf;
	buf.push_back( line[ i++ ] );
	while( i < line_len ) {
		if( !isalnum( CURR( line ) ) && CURR( line ) != '_' ) break;
		buf.push_back( line[ i++ ] );
	}
	return buf;
}

static int classify_str( const std::string & str )
{
	if( str == TokStrs[ TOK_TRUE ] ) return TOK_TRUE;
	else if( str == TokStrs[ TOK_FALSE ] ) return TOK_FALSE;
	else if( str == TokStrs[ TOK_ENUM ] ) return TOK_ENUM;
	else if( str == TokStrs[ TOK_IMPORT ] ) return TOK_IMPORT;
	else if( str == TokStrs[ TOK_AS ] ) return TOK_AS;
	else if( str == TokStrs[ TOK_NIL ] ) return TOK_NIL;
	else if( str == TokStrs[ TOK_STRUCT ] ) return TOK_STRUCT;
	else if( str == TokStrs[ TOK_FN ] ) return TOK_FN;
	else if( str == TokStrs[ TOK_MFN ] ) return TOK_MFN;
	else if( str == TokStrs[ TOK_RETURN ] ) return TOK_RETURN;
	else if( str == TokStrs[ TOK_IF ] ) return TOK_IF;
	else if( str == TokStrs[ TOK_ELIF ] ) return TOK_ELIF;
	else if( str == TokStrs[ TOK_ELSE ] ) return TOK_ELSE;
	else if( str == TokStrs[ TOK_FOR ] ) return TOK_FOR;
	else if( str == TokStrs[ TOK_CONTINUE ] ) return TOK_CONTINUE;
	else if( str == TokStrs[ TOK_BREAK ] ) return TOK_BREAK;
	else if( str == TokStrs[ TOK_LDMOD ] ) return TOK_LDMOD;
	else if( str == TokStrs[ TOK_SELF ] ) return TOK_SELF;

	return TOK_IDEN;
}

static std::string get_num( const std::string & line, const int line_len, const int line_num, int & i, int & num_type )
{
	std::string buf;
	int first_digit_at = i;

	bool success = true;
	int dot_encountered = -1;

	while( i < line_len && is_valid_num_char( CURR( line ) ) ) {
		const char c = CURR( line );
		const char next = NEXT( line );
		switch( c ) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		case '.':
			if( dot_encountered == -1 ) {
				if( next >= '0' && next <= '9' ) {
					dot_encountered = i;
					num_type = TOK_FLT;
				} else {
					return buf;
				}
			} else {
				SRC_FAIL( "encountered dot (.) character "
					  "when the number being retrieved (from column %d) "
					  "already had one at column %d",
					  first_digit_at + 1, dot_encountered + 1 );
				success = false;
			}
			break;
		default:
			if( isalnum( CURR( line ) ) ) {
				SRC_FAIL( "encountered invalid character '%c' "
					  "while retrieving a number (from column %d)",
					  c, first_digit_at + 1 );
				success = false;
			} else {
				return buf;
			}
			
		}
		if( success == false ) {
			return "";
		}
		buf.push_back( c ); ++i;
	}
	return buf;
}

static int get_const_str( const std::string & line, const int line_len, const int line_num, int & i, std::string & buf )
{
	buf.clear();
	const char quote_type = CURR( line );
	int starting_at = i;
	// omit beginning quote
	++i;
	while( i < line_len ) {
		if( CURR( line ) == quote_type && PREV( line ) != '\\' ) break;

		buf.push_back( line[ i++ ] );
	}
	if( CURR( line ) != quote_type ) {
		i = starting_at;
		SRC_FAIL( "no matching quote for '%c' found",
			  quote_type );
		return E_LEX_FAIL;
	}
	// omit ending quote
	++i;
	return E_OK;
}

static int get_operator( const std::string & line, const int line_len, const int line_num, int & i )
{
	int op_type = -1;
	switch( CURR( line ) ) {
	case '+':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				op_type = TOK_ADD_ASSN;
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_ADD );
	case '-':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				op_type = TOK_SUB_ASSN;
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_SUB );
	case '*':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '*' || NEXT( line ) == '=' ) {
				++i;
				if( CURR( line ) == '*' ) op_type = TOK_POW;
				else if( CURR( line ) == '=' ) op_type = TOK_MUL_ASSN;
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_MUL );
	case '/':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				SET_OP_TYPE_BRK( TOK_DIV_ASSN );
			}
		}
		SET_OP_TYPE_BRK( TOK_DIV );
	case '%':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				SET_OP_TYPE_BRK( TOK_MOD_ASSN );
			}
		}
		SET_OP_TYPE_BRK( TOK_MOD );
	case '&':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '&' || NEXT( line ) == '=' ) {
				++i;
				if( CURR( line ) == '&' ) op_type = TOK_AND;
				else if( CURR( line ) == '=' ) op_type = TOK_BAND_ASSN;
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_BAND );
	case '|':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '|' || NEXT( line ) == '=' ) {
				++i;
				if( CURR( line ) == '|' ) op_type = TOK_OR;
				else if( CURR( line ) == '=' ) op_type = TOK_BOR_ASSN;
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_BOR );
	case '~':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				SET_OP_TYPE_BRK( TOK_BNOT_ASSN );
			}
		}
		SET_OP_TYPE_BRK( TOK_BNOT );
	case '=':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				SET_OP_TYPE_BRK( TOK_EQ );
			}
		}
		SET_OP_TYPE_BRK( TOK_ASSN );
	case '<':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' || NEXT( line ) == '<' ) {
				++i;
				if( CURR( line ) == '=' ) op_type = TOK_LE;
				else if( CURR( line ) == '<' ) {
					if( i < line_len - 1 ) {
						if( NEXT( line ) == '=' ) {
							++i;
							SET_OP_TYPE_BRK( TOK_LSHIFT_ASSN );
						}
					}
					op_type = TOK_LSHIFT;
				}
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_LT );
	case '>':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' || NEXT( line ) == '>' ) {
				++i;
				if( CURR( line ) == '=' ) op_type = TOK_GE;
				else if( CURR( line ) == '>' ) {
					if( i < line_len - 1 ) {
						if( NEXT( line ) == '=' ) {
							++i;
							SET_OP_TYPE_BRK( TOK_RSHIFT_ASSN );
						}
					}
					op_type = TOK_RSHIFT;
				}
				break;
			}
		}
		SET_OP_TYPE_BRK( TOK_GT );
	case '!':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				SET_OP_TYPE_BRK( TOK_NE );
			}
		}
		SET_OP_TYPE_BRK( TOK_NOT );
	case '^':
		if( i < line_len - 1 ) {
			if( NEXT( line ) == '=' ) {
				++i;
				SET_OP_TYPE_BRK( TOK_BXOR_ASSN );
			}
		}
		SET_OP_TYPE_BRK( TOK_BXOR );
	case '?':
		SET_OP_TYPE_BRK( TOK_QUEST );
	case ' ':
		SET_OP_TYPE_BRK( TOK_SPC );
	case '\t':
		SET_OP_TYPE_BRK( TOK_TAB );
	case '\n':
		SET_OP_TYPE_BRK( TOK_NEWL );
	case '.':
		SET_OP_TYPE_BRK( TOK_DOT );
	case ',':
		SET_OP_TYPE_BRK( TOK_COMMA );
	case ':':
		SET_OP_TYPE_BRK( TOK_COL );
	case ';':
		SET_OP_TYPE_BRK( TOK_COLS );
	case '@':
		SET_OP_TYPE_BRK( TOK_AT );
	case '(':
		SET_OP_TYPE_BRK( TOK_LPAREN );
	case '[':
		SET_OP_TYPE_BRK( TOK_LBRACK );
	case '{':
		SET_OP_TYPE_BRK( TOK_LBRACE );
	case ')':
		SET_OP_TYPE_BRK( TOK_RPAREN );
	case ']':
		SET_OP_TYPE_BRK( TOK_RBRACK );
	case '}':
		SET_OP_TYPE_BRK( TOK_RBRACE );
	default:
		SRC_FAIL( "unknown operator '%c' found",
			  CURR( line ) );
		op_type = -1;
	}

	++i;
	return op_type;
}

static inline bool is_valid_num_char( const char c )
{
	return ( c >= '0' && c <= '9' ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' )
		|| c == '.' || c == '-' || c == '+' || c == 'o' || c == 'O' || c == 'x' || c == 'X';
}