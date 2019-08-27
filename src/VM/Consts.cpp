/*
  Copyright (c) 2019, Electrux
  All rights reserved.
  Using the BSD 3-Clause license for the project,
  main LICENSE file resides in project's root directory.
  Please read that file and understand the license terms
  before using or altering the project.
*/

#include "Consts.hpp"

consts_t::consts_t() {}
consts_t::~consts_t()
{
	for( auto & t : m_consts ) {
		for( auto & cons : t.second ) {
			VAR_DREF( cons.second );
		}
	}
}

var_base_t * consts_t::get( const std::string & name, const OperTypes type, const int tok_ctr )
{
	if( m_consts[ type ].find( name ) != m_consts[ type ].end() ) {
		if( name == m_consts[ type ][ name ]->to_str() ) {
			return m_consts[ type ][ name ];
		}
		VAR_DREF( m_consts[ type ][ name ] );
	}

	if( type == OP_INT ) return m_consts[ type ][ name ] = new var_int_t( name, tok_ctr );
	else if( type == OP_FLT ) return m_consts[ type ][ name ] = new var_flt_t( name, tok_ctr );
	else if( type == OP_STR || type == OP_CONST ) return m_consts[ type ][ name ] = new var_str_t( name, tok_ctr );

	return nullptr;
}