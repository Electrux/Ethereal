/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_FUNCTIONS_HPP
#define PARSER_FUNCTIONS_HPP

#include "Common.hpp"

#define NEXT_VALID( tok )								\
do {											\
	if( ph.peak( 1 )->type == TOK_INVALID ) {					\
		PARSE_FAIL( "expected token '%s', found <EOF>", TokStrs[ tok ] );	\
		return nullptr;								\
	} else {									\
		ph.next();								\
	}										\
} while( 0 )

#define NEXT_VALID2( tok1, tok2 )										\
do {														\
	if( ph.peak( 1 )->type == TOK_INVALID ) {								\
		PARSE_FAIL( "expected token '%s' or '%s', found <EOF>", TokStrs[ tok1 ], TokStrs[ tok2 ] );	\
		return nullptr;											\
	} else {												\
		ph.next();											\
	}													\
} while( 0 )

stmt_enum_t * parse_enum( const src_t & src, parse_helper_t & ph );

#endif // PARSER_FUNCTIONS_HPP