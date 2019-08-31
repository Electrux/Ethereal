/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <sys/ioctl.h>

#include "../src/VM/Core.hpp"

static std::unordered_map< std::string, const char * > COL = {
	{ "0", "\033[0m" },

	{ "r", "\033[0;31m" },
	{ "g", "\033[0;32m" },
	{ "y", "\033[0;33m" },
	{ "b", "\033[0;34m" },
	{ "m", "\033[0;35m" },
	{ "c", "\033[0;36m" },
	{ "w", "\033[0;37m" },

	{ "br", "\033[1;31m" },
	{ "bg", "\033[1;32m" },
	{ "by", "\033[1;33m" },
	{ "bb", "\033[1;34m" },
	{ "bm", "\033[1;35m" },
	{ "bc", "\033[1;36m" },
	{ "bw", "\033[1;37m" },
};

void apply_colors( std::string & str )
{
	for( size_t i = 0; i < str.size(); ) {
		if( str[ i ] == '{' && ( i == 0 || ( str[ i - 1 ] != '$' && str[ i - 1 ] != '%' && str[ i - 1 ] != '#' && str[ i - 1 ] != '\\' ) ) ) {
			str.erase( str.begin() + i );
			if( i < str.size() && str[ i ] == '{' ) {
				++i;
				continue;
			}

			std::string var;

			while( i < str.size() && str[ i ] != '}' ) {
				var += str[ i ];
				str.erase( str.begin() + i );
			}

			// Remove the ending brace
			if( i < str.size() ) str.erase( str.begin() + i );

			if( var.empty() ) continue;

			std::string val = COL[ var ];

			str.insert( str.begin() + i, val.begin(), val.end() );
			i += val.size();
		}
		else {
			++i;
		}
	}
}

var_base_t * colorize( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string data = fcd.args[ 0 ]->to_str();
	apply_colors( data );
	return new var_str_t( data );
}

var_base_t * col_print( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		std::string data = v->to_str();
		apply_colors( data );
		fprintf( stdout, "%s", data.c_str() );
	}
	return nullptr;
}

var_base_t * col_println( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		std::string data = v->to_str();
		apply_colors( data );
		fprintf( stdout, "%s", data.c_str() );
	}
	fprintf( stdout, "\n" );
	return nullptr;
}

var_base_t * term_size( vm_state_t & vm, func_call_data_t & fcd )
{
	int rows = 80, cols = 24;

#ifdef TIOCGSIZE
	struct ttysize ts;
	ioctl( 0, TIOCGSIZE, & ts );
	cols = ts.ts_cols;
	rows = ts.ts_lines;
#elif defined TIOCGWINSZ
	struct winsize ws;
	ioctl( 0, TIOCGWINSZ, & ws );
	cols = ws.ws_col;
	rows = ws.ws_row;
#endif /* TIOCGSIZE */

	std::unordered_map< std::string, var_base_t * > term_sz = {
		{ "rows", new var_int_t( rows, fcd.parse_ctr ) },
		{ "cols", new var_int_t( cols, fcd.parse_ctr ) }
	};
	// _term_size_t struct is defined in include/ethereal/term.et
	return new var_struct_t( "_term_size_t", term_sz );
}

REGISTER_MODULE( term )
{
	vm.funcs.add( { "colorize", 1,  1, { "str" }, FnType::MODULE, { .modfn = colorize }, true } );
	vm.funcs.add( { "cprint",   1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = col_print }, false } );
	vm.funcs.add( { "cprintln", 0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = col_println }, false } );

	functions_t & termfns = vm.typefuncs[ "_term_t" ];
	termfns.add( { "size", 0,  0, {}, FnType::MODULE, { .modfn = term_size }, true } );
}