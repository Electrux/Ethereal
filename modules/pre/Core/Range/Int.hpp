/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_RANGE_INT_HPP
#define VM_MODULES_CORE_RANGE_INT_HPP

#include "../../../../src/VM/Core.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Classes ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class range_iter_int_t : public var_base_t
{
	mpz_class m_beg, m_end;
	mpz_class m_step;
	mpz_class m_curr;
public:
	range_iter_int_t( const mpz_class & beg, const mpz_class & end, const mpz_class & step, const mpz_class & curr,
			  const int src_idx = 0, const int parse_ctr = 0 );
	~range_iter_int_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );

	mpz_class & get_beg();
	mpz_class & get_end();
	mpz_class & get_step();
	mpz_class & get_curr();
};
#define AS_RANGE_ITER_INT( x ) static_cast< range_iter_int_t * >( x )

range_iter_int_t::range_iter_int_t( const mpz_class & beg, const mpz_class & end, const mpz_class & step, const mpz_class & curr,
							const int src_idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, src_idx, parse_ctr ), m_beg( beg ), m_end( end ), m_step( step ), m_curr( curr ) {}
range_iter_int_t::~range_iter_int_t() {}

std::string range_iter_int_t::type_str() const { return "range_iter_int_t"; }
std::string range_iter_int_t::to_str() const
{
	return m_curr.get_str();
}
mpz_class range_iter_int_t::to_int() const { return m_curr; }
bool range_iter_int_t::to_bool() const { return m_curr >= m_beg && m_curr < m_end; }
var_base_t * range_iter_int_t::copy( const int src_idx, const int parse_ctr )
{
	return new range_iter_int_t( m_beg, m_end, m_step, m_curr, src_idx, parse_ctr );
}
void range_iter_int_t::assn( var_base_t * b )
{
	m_beg = AS_RANGE_ITER_INT( b )->get_beg();
	m_end = AS_RANGE_ITER_INT( b )->get_end();
	m_step = AS_RANGE_ITER_INT( b )->get_step();
	m_curr = AS_RANGE_ITER_INT( b )->get_curr();
}
mpz_class & range_iter_int_t::get_beg() { return m_beg; }
mpz_class & range_iter_int_t::get_end() { return m_end; }
mpz_class & range_iter_int_t::get_step() { return m_step; }
mpz_class & range_iter_int_t::get_curr() { return m_curr; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_base_t * range_int( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 0 ] )->get();
	mpz_class & b = AS_INT( fcd.args[ 1 ] )->get();
	mpz_class step = fcd.args.size() > 2 ? AS_INT( fcd.args[ 2 ] )->get() : 1;
	return new range_iter_int_t( a, b, step, a - step );
}

var_base_t * next_int( vm_state_t & vm, func_call_data_t & fcd )
{
	range_iter_int_t * it = AS_RANGE_ITER_INT( fcd.args[ 0 ] );
	const mpz_class & a = it->get_beg();
	const mpz_class & b = it->get_end();
	const mpz_class & s = it->get_step();
	mpz_class & i = it->get_curr();
	i += s;
	if( i >= b ) return vm.nil;
	return new var_int_t( i );
}

#endif // VM_MODULES_CORE_RANGE_INT_HPP