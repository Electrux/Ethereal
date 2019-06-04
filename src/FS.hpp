/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef FS_HPP
#define FS_HPP

#include <vector>
#include <string>

#include "Ethereal.hpp"

int read_file( eth_t & eth );

int get_internal_file_path( std::string & file, const std::string & type );

bool fexists( const std::string & file );

#endif // FS_HPP
