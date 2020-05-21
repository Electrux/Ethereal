/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

var_none_t::var_none_t( const int src_idx, const int parse_ctr )
	: var_base_t( VT_NONE, true, src_idx, parse_ctr ) {}

std::string var_none_t::to_str() const { return VarTypeStrs[ VT_NONE ]; }
mpz_class var_none_t::to_int() const { return 0; }
bool var_none_t::to_bool() const { return false; }

var_base_t * var_none_t::copy( const int src_idx, const int parse_ctr )
	{ return new var_none_t( src_idx, parse_ctr ); }
void var_none_t::assn( var_base_t * b ) {}