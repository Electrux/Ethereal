/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

const char * TypeStrs[ Types::_TYPE_LAST ] = {
	"STR",
	"FLT",
	"INT",
	"BOOL",
	"VEC",
	"MAP",
	"BLOCK",
	"FUNC",
	"MOD",
};

////////////////////////////////////////////////////////////
///////////////////////// var_base_t //////////////////////////
////////////////////////////////////////////////////////////

var_base_t::var_base_t( const int type ) : m_type( type ), m_ref_count( 1 ) {}
var_base_t::~var_base_t() {}
int var_base_t::type() const { return m_type; }
int var_base_t::ref_count() const { return m_ref_count; }
