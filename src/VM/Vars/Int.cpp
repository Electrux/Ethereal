#include "Base.hpp"

var_int_t::var_int_t( const int val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const std::string & val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const bool val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const float val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const mpz_class & val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}
var_int_t::var_int_t( const mpf_class & val, const int parse_ctr )
	: var_base_t( VT_INT, parse_ctr ), m_val( val ) {}

std::string var_int_t::to_str() const { return m_val.get_str(); }
mpz_class var_int_t::to_int() const { return m_val; }
bool var_int_t::to_bool() const { return m_val.get_si(); }
void * var_int_t::get() { return & m_val; }