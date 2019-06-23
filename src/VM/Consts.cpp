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
	for( auto & cons : m_consts ) {
		VAR_DREF( cons.second );
	}
}

var_base_t * consts_t::get( const instr_t & ins )
{
	if( m_consts.find( ins.oper.val ) != m_consts.end() && ins.oper.type == m_consts[ ins.oper.val ]->type() ) {
		return m_consts[ ins.oper.val ];
	}
	if( m_consts.find( ins.oper.val ) != m_consts.end() ) VAR_DREF( m_consts[ ins.oper.val ] );
	var_base_t * tmp = nullptr;

	if( ins.oper.type == TYPE_STR ) tmp = new var_str_t( ins.oper.val );
	else if( ins.oper.type == TYPE_INT ) tmp = new var_int_t( ins.oper.val );
	else if( ins.oper.type == TYPE_FLT ) tmp = new var_flt_t( ins.oper.val );
	else if( ins.oper.type == TYPE_BOOL ) tmp = new var_bool_t( ins.oper.val );
	else return nullptr;

	m_consts[ ins.oper.val ] = tmp;
	return m_consts[ ins.oper.val ];
}