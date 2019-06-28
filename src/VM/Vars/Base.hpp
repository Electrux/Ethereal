/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_VARS_BASE_HPP
#define VM_VARS_BASE_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <gmpxx.h>

#include "../Consts.hpp"
#include "../Vars.hpp"
#include "../Aliases.hpp"
#include "../Functions.hpp"

enum Types
{
	TYPE_STR,
	TYPE_FLT,
	TYPE_INT,
	TYPE_BOOL,
	TYPE_VEC,
	TYPE_MAP,
	TYPE_STRUCT,
	TYPE_BLOCK,
	TYPE_FUNC,
	TYPE_MOD,

	_TYPE_LAST,
};

extern const char * TypeStrs[ _TYPE_LAST ];

class var_base_t
{
	int m_type;
	int m_ref_count;
	funcs_t m_mfns;
public:
	var_base_t( const int type );
	virtual ~var_base_t();

	int type() const;
	int ref_count() const;
	funcs_t * mem_funcs() { return & m_mfns; }

	inline void inc_ref_count()
	{
		++m_ref_count;
	}

	inline void dec_ref_count()
	{
		--m_ref_count;
	}

	virtual std::string type_str() const = 0;
	virtual std::string to_str() const = 0;
	virtual mpz_class to_int() const = 0;
	virtual bool to_bool() const = 0;
	virtual var_base_t * copy() const = 0;
};

#define VAR_IREF( var )					\
	do { ( var )->inc_ref_count(); } while( 0 )

#define VAR_IREF_VOIDPTR( var )				\
	do { ( ( var_base_t * )var )->inc_ref_count(); } while( 0 )

#define VAR_DREF( var )					\
	do {							\
		( var )->dec_ref_count();			\
		if( ( var )->ref_count() <= 0 ) {		\
			delete ( var );				\
			var = nullptr;				\
		}						\
	} while( 0 )

#define VAR_DREF_VOIDPTR( var )						\
	do {								\
		( ( var_base_t * )var )->dec_ref_count();		\
		if( ( ( var_base_t * )var )->ref_count() <= 0 ) {	\
			delete ( var_base_t * )var;			\
			var = nullptr;					\
		}							\
	} while( 0 )

class var_str_t : public var_base_t
{
	std::string m_val;
	const bool m_from_subscr_load;
public:
	var_str_t( const std::string & val, const bool from_subscr_load = false );
	std::string & Get();
	bool FromSubscrLoad();
	~var_str_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_flt_t : public var_base_t
{
	mpf_class m_val;
public:
	var_flt_t( const mpf_class & val );
	var_flt_t( const std::string & val );
	mpf_class & Get();
	~var_flt_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_int_t : public var_base_t
{
	mpz_class m_val;
public:
	var_int_t( const mpz_class & val );
	var_int_t( const std::string & val );
	mpz_class & Get();
	~var_int_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_bool_t : public var_base_t
{
	bool m_val;
public:
	var_bool_t( const bool & val );
	var_bool_t( const std::string & val );
	bool & Get();
	~var_bool_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_block_t : public var_base_t
{
	const size_t m_start, m_end;
public:
	var_block_t( size_t start,
		  size_t end );
	~var_block_t();
	size_t start();
	size_t end();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_vector_t : public var_base_t
{
	std::vector< var_base_t * > m_vec;
public:
	var_vector_t( const std::vector< var_base_t * > & vec );
	~var_vector_t();
	std::vector< var_base_t * > & Get();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_map_t : public var_base_t
{
	std::unordered_map< std::string, var_base_t * > m_map;
public:
	var_map_t();
	var_map_t( std::unordered_map< std::string, var_base_t * > & map );
	~var_map_t();
	std::unordered_map< std::string, var_base_t * > & Get();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_struct_t : public var_base_t
{
	std::string m_name;
	std::vector< std::string > m_field_locs;
	std::unordered_map< std::string, var_base_t * > m_fields;
	const bool m_is_def;
public:
	var_struct_t( const std::string & name, const std::vector< std::string > & field_locs,
		   const std::unordered_map< std::string, var_base_t * > fields, const bool is_def );
	~var_struct_t();
	const std::string & GetName() const;
	const std::vector< std::string > & GetFieldLocs() const;
	std::unordered_map< std::string, var_base_t * > & GetFields();
	bool IsDef() const;
	void Swap( var_struct_t * var );
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

class var_func_t : public var_base_t
{
	std::string m_name;
	var_vector_t * m_args;
	var_block_t * m_body;
public:
	var_func_t( const std::string & name, var_vector_t * args, var_block_t * body );
	var_func_t( var_vector_t * args, var_block_t * body );
	~var_func_t();
	void set_name( const std::string & name );
	const std::string & get_name();
	var_vector_t * get_args();
	var_block_t * get_body();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

struct src_t;
class var_mod_t;
typedef std::unordered_map< std::string, var_mod_t * > modules_t;

class var_mod_t : public var_base_t
{
	std::string m_name;
	src_t & m_src;
	consts_t m_consts;
	vars_t m_vars;
	aliases_t m_aliases;
	modules_t m_mods;
public:
	var_mod_t( const std::string & name, src_t & src );
	~var_mod_t();
	src_t & src();
	consts_t & consts();
	vars_t & vars();
	aliases_t & aliases();
	modules_t & mods();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
};

#define REGISTER_MODULE( name )				\
	extern "C" void init_##name( modules_t & mods )

typedef void ( * init_fn )( modules_t & mods );

inline bool is_valid_num_char( const char c )
{
	return ( c >= '0' && c <= '9' ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' ) ||
	       c == '-' || c == '+' || c == 'o' || c == 'O' || c == 'x' || c == 'X';
}

inline bool is_valid_flt_char( const char c )
{
	return ( c >= '0' && c <= '9' ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' ) ||
	       c == '.' || c == '-' || c == '+' || c == 'o' || c == 'O' || c == 'x' || c == 'X';
}

inline bool is_num( const std::string & str )
{
	return std::all_of( str.begin(), str.end(), ::is_valid_num_char );
}

inline bool is_flt( const std::string & str )
{
	return std::all_of( str.begin(), str.end(), ::is_valid_flt_char );
}

#endif // VM_VARS_BASE_HPP
