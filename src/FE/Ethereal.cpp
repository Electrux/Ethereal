/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Ethereal.hpp"

src_t::src_t( const bool _is_main_src )
	: ptree( nullptr ), is_main_src( _is_main_src ),
	  found_assn( false ), bcode_as_const( false ) {}

src_t::~src_t()
{
	if( ptree != nullptr ) {
		for( auto & stmt : * ptree ) delete stmt;
		delete ptree;
	}
}