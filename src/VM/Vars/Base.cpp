/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

const char * VarTypeStrs[ _VT_LAST ] = {
	"int",
	"str",
	"flt",
	"bool",

	"enum",
	"vec",
	"map",

	"func",
};

var_base_t::var_base_t( const VarType type, const int parse_ctr )
	: m_type( type ), m_ref_ctr( 1 ), m_parse_ctr( parse_ctr ) {}

var_base_t::~var_base_t() {}

VarType var_base_t::type() const { return m_type; }
int var_base_t::ref() const { return m_ref_ctr; }
int var_base_t::parse_ctr() const { return m_parse_ctr; }

std::string var_base_t::type_str() const { return VarTypeStrs[ m_type ]; }