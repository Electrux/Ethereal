/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Core.hpp"
#include "../Ethereal.hpp"

vm_src_t::vm_src_t( const std::string & _name, src_t & _src, const int _begin, const int _end )
	: name( _name ), src( _src ), begin( _begin ), end( _end == -1 ? _src.bcode.size() : _end ) {}
