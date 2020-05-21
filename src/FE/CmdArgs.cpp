/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstring>

#include "CmdArgs.hpp"

const size_t OPT_A = 1 << 0;
const size_t OPT_B = 1 << 1; // show byte code
const size_t OPT_C = 1 << 2; // (byte) compile
const size_t OPT_D = 1 << 3; // dry run (no execute)
const size_t OPT_E = 1 << 4; // REPL (eval)
const size_t OPT_F = 1 << 5;
const size_t OPT_G = 1 << 6;
const size_t OPT_H = 1 << 7;
const size_t OPT_I = 1 << 8;
const size_t OPT_L = 1 << 9;
const size_t OPT_P = 1 << 10; // show parse tree
const size_t OPT_R = 1 << 11; // recursively show everything (ex. FrontEnd->VM->Import->FrontEnd...)
const size_t OPT_S = 1 << 12;
const size_t OPT_T = 1 << 13; // show tokens
const size_t OPT_V = 1 << 14; // show version
const size_t OPT_1 = 1 << 15;

size_t cmd_get_args( const int argc, const char ** argv, std::vector< std::string > & args )
{
	size_t flags = 0;
	bool src_found = false;

	for( int i = 1; i < argc; ++i ) {
		if( argv[ i ][ 0 ] != '-' || src_found ) {
			src_found = true;
			args.push_back( argv[ i ] );
			continue;
		}

		size_t len = strlen( argv[ i ] );
		for( size_t j = 1; j <= len; ++j ) {
			if( argv[ i ][ j ] == 'a' ) flags |= OPT_A;
			else if( argv[ i ][ j ] == 'b' ) flags |= OPT_B;
			else if( argv[ i ][ j ] == 'c' ) flags |= OPT_C;
			else if( argv[ i ][ j ] == 'd' ) flags |= OPT_D;
			else if( argv[ i ][ j ] == 'e' ) flags |= OPT_E;
			else if( argv[ i ][ j ] == 'f' ) flags |= OPT_F;
			else if( argv[ i ][ j ] == 'g' ) flags |= OPT_G;
			else if( argv[ i ][ j ] == 'h' ) flags |= OPT_H;
			else if( argv[ i ][ j ] == 'i' ) flags |= OPT_I;
			else if( argv[ i ][ j ] == 'l' ) flags |= OPT_L;
			else if( argv[ i ][ j ] == 'p' ) flags |= OPT_P;
			else if( argv[ i ][ j ] == 'r' ) flags |= OPT_R;
			else if( argv[ i ][ j ] == 's' ) flags |= OPT_S;
			else if( argv[ i ][ j ] == 't' ) flags |= OPT_T;
			else if( argv[ i ][ j ] == 'v' ) flags |= OPT_V;
			else if( argv[ i ][ j ] == '1' ) flags |= OPT_1;
		}
	}

	return flags;
}
