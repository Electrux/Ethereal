/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_PARSE_HELPER_HPP
#define PARSER_PARSE_HELPER_HPP

#include "../Ethereal.hpp"

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
	void set_tok_ctr( const int tok_ctr );
};

#endif // PARSER_PARSE_HELPER_HPP