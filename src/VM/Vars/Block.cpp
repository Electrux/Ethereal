/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_block_t::var_block_t( const size_t start,
		    const size_t end )
	: var_base_t( TYPE_BLOCK ), m_start( start ), m_end( end )
{}
var_block_t::~var_block_t() {}

size_t var_block_t::start() { return m_start; }
size_t var_block_t::end() { return m_end; }
std::string var_block_t::type_str() const { return TypeStrs[ TYPE_BLOCK ]; }
std::string var_block_t::to_str() const { return ""; }
mpz_class var_block_t::to_int() const { return m_end - m_start; }
bool var_block_t::to_bool() const { return m_end - m_start > 0; }
var_base_t * var_block_t::copy() const { return new var_block_t( m_start, m_end ); }
