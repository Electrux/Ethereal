#include "../../Ethereal.hpp"
#include "Base.hpp"

var_mod_t::var_mod_t( const std::string & name, src_t & src )
	: var_base_t( TYPE_MOD ), m_name( name ), m_src( src ) {}

var_mod_t::~var_mod_t() {}

src_t & var_mod_t::src() { return m_src; }
consts_t & var_mod_t::consts() { return m_consts; }
vars_t & var_mod_t::vars() { return m_vars; }
aliases_t & var_mod_t::aliases() { return m_aliases; }
modules_t & var_mod_t::mods() { return m_mods; }

std::string var_mod_t::type_str() const { return std::string( TypeStrs[ TYPE_MOD ] ) + "(" + m_name + ")"; }
std::string var_mod_t::to_str() const
{
	std::string modinfo = "< module: " + m_name + " >{ ";
	modinfo += "consts: " + std::to_string( m_consts.size() ) + ", ";
	modinfo += "mods: " + std::to_string( m_mods.size() ) + ", ";
	modinfo += "var layers: " + std::to_string( m_vars.layer_count() ) + ", ";
	modinfo += "aliases: " + std::to_string( m_aliases.count() ) + " }";
	return modinfo;
}

mpz_class var_mod_t::to_int() const { return 1; }
bool var_mod_t::to_bool() const { return true; }
var_base_t * var_mod_t::copy() const { return nullptr; }