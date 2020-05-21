/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_RANGE_FLT_HPP
#define VM_MODULES_CORE_RANGE_FLT_HPP

#include "../../../../src/VM/Core.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class range_iter_flt_t : public var_base_t
{
	mpfr::mpreal m_beg, m_end;
	mpfr::mpreal m_step;
	mpfr::mpreal m_curr;
public:
	range_iter_flt_t( const mpfr::mpreal & beg, const mpfr::mpreal & end, const mpfr::mpreal & step, const mpfr::mpreal & curr,
			  const int src_idx = 0, const int parse_ctr = 0 );
	~range_iter_flt_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );

	mpfr::mpreal & get_beg();
	mpfr::mpreal & get_end();
	mpfr::mpreal & get_step();
	mpfr::mpreal & get_curr();
};
#define AS_RANGE_ITER_FLT( x ) static_cast< range_iter_flt_t * >( x )

range_iter_flt_t::range_iter_flt_t( const mpfr::mpreal & beg, const mpfr::mpreal & end, const mpfr::mpreal & step, const mpfr::mpreal & curr,
							const int src_idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, src_idx, parse_ctr ), m_beg( beg ), m_end( end ), m_step( step ), m_curr( curr ) {}
range_iter_flt_t::~range_iter_flt_t() {}

std::string range_iter_flt_t::type_str() const { return "range_iter_flt_t"; }
std::string range_iter_flt_t::to_str() const
{
	return m_curr.toString();
}
mpz_class range_iter_flt_t::to_int() const { return mpz_class( mpf_class( m_curr.toString() ) ); }
bool range_iter_flt_t::to_bool() const { return m_curr >= m_beg && m_curr < m_end; }
var_base_t * range_iter_flt_t::copy( const int src_idx, const int parse_ctr )
{
	return new range_iter_flt_t( m_beg, m_end, m_step, m_curr, src_idx, parse_ctr );
}
void range_iter_flt_t::assn( var_base_t * b )
{
	m_beg = AS_RANGE_ITER_FLT( b )->get_beg();
	m_end = AS_RANGE_ITER_FLT( b )->get_end();
	m_step = AS_RANGE_ITER_FLT( b )->get_step();
	m_curr = AS_RANGE_ITER_FLT( b )->get_curr();
}
mpfr::mpreal & range_iter_flt_t::get_beg() { return m_beg; }
mpfr::mpreal & range_iter_flt_t::get_end() { return m_end; }
mpfr::mpreal & range_iter_flt_t::get_step() { return m_step; }
mpfr::mpreal & range_iter_flt_t::get_curr() { return m_curr; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * range_flt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & a = AS_FLT( fcd.args[ 0 ] )->get();
	mpfr::mpreal & b = AS_FLT( fcd.args[ 1 ] )->get();
	mpfr::mpreal step = fcd.args.size() > 2 ? AS_FLT( fcd.args[ 2 ] )->get() : 1.0;
	return new range_iter_flt_t( a, b, step, a - step );
}

var_base_t * next_flt( vm_state_t & vm, func_call_data_t & fcd )
{
	range_iter_flt_t * it = AS_RANGE_ITER_FLT( fcd.args[ 0 ] );
	const mpfr::mpreal & a = it->get_beg();
	const mpfr::mpreal & b = it->get_end();
	const mpfr::mpreal & s = it->get_step();
	mpfr::mpreal & i = it->get_curr();
	i += s;
	if( i >= b ) return vm.nil;
	return new var_flt_t( i );
}

#endif // VM_MODULES_CORE_RANGE_FLT_HPP