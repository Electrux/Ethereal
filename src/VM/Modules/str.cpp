/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../Vars/Base.hpp"
#include "../Core.hpp"

var_base_t * len( std::vector< var_base_t * > & vars )
{
	return new var_int_t( ( int )AS_STR( vars[ 0 ] )->get().size(), vars[ 0 ]->parse_ctr() );
}

var_base_t * clear( std::vector< var_base_t * > & vars )
{
	AS_STR( vars[ 0 ] )->get().clear();
	return nullptr;
}

REGISTER_MODULE( str )
{
	functions_t & strfns = vm.typefuncs[ "str" ];

	strfns.add( { "len", 0, 0, {}, FnType::MODULE, { .modfn = len } } );
	strfns.add( { "clear", 0, 0, {}, FnType::MODULE, { .modfn = clear } } );
}