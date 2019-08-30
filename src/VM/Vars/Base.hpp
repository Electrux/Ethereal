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

#include <string>
#include <gmpxx.h>

#include "../Functions.hpp"

enum VarType
{
	VT_NONE,
	VT_NIL, // difference between NONE and NIL is that NONE is used internally, while NIL can be used in code
	VT_INT,
	VT_STR,
	VT_FLT,
	VT_BOOL,

	VT_ENUM,
	VT_VEC,
	VT_MAP,
	VT_TUPLE,

	VT_STRUCT,

	VT_CUSTOM,

	_VT_LAST,
};

extern const char * VarTypeStrs[ _VT_LAST ];

class var_base_t
{
	VarType m_type;
	int m_ref_ctr;
	int m_parse_ctr;
	bool m_implements_assign;
public:
	var_base_t( const VarType type, const bool implements_assign, const int parse_ctr = 0 );
	virtual ~var_base_t();

	VarType type() const;
	int ref() const;
	void set_parse_ctr( const int parse_ctr );
	int parse_ctr() const;

	inline void inc_ref() { ++m_ref_ctr; }
	inline void dec_ref() { --m_ref_ctr; }
	inline bool impls_assn() { return m_implements_assign; }

	virtual std::string type_str() const;
	virtual std::string to_str() const = 0;
	virtual mpz_class to_int() const = 0;
	virtual bool to_bool() const = 0;
	virtual var_base_t * copy( const int parse_ctr ) = 0;
	virtual void assn( var_base_t * b );
};

#define VAR_IREF( var ) do { ( ( var_base_t * )var )->inc_ref(); } while( 0 )

#define VAR_DREF( var )						\
	do {							\
		( ( var_base_t * )var )->dec_ref();		\
		if( ( ( var_base_t * )var )->ref() <= 0 ) {	\
			delete ( var_base_t * )var;		\
			var = nullptr;				\
		}						\
	} while( 0 )

class var_none_t : public var_base_t
{
public:
	var_none_t( const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
};
#define AS_NONE( x ) static_cast< var_none_t * >( x )

class var_nil_t : public var_base_t
{
public:
	var_nil_t( const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
};
#define AS_NIL( x ) static_cast< var_nil_t * >( x )

class var_int_t : public var_base_t
{
	mpz_class m_val;
public:
	var_int_t( const int val, const int parse_ctr = 0 );
	var_int_t( const std::string & val, const int parse_ctr = 0 );
	var_int_t( const bool val, const int parse_ctr = 0 );
	var_int_t( const float val, const int parse_ctr = 0 );
	var_int_t( const mpz_class & val, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	mpz_class & get();
};
#define AS_INT( x ) static_cast< var_int_t * >( x )

class var_str_t : public var_base_t
{
	std::string m_val;
public:
	var_str_t( const std::string & val, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	std::string & get();
};
#define AS_STR( x ) static_cast< var_str_t * >( x )

class var_flt_t : public var_base_t
{
	mpf_class m_val;
public:
	var_flt_t( const float val, const int parse_ctr = 0 );
	var_flt_t( const int val, const int parse_ctr = 0 );
	var_flt_t( const std::string & val, const int parse_ctr = 0 );
	var_flt_t( const bool val, const int parse_ctr = 0 );
	var_flt_t( const mpf_class & val, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	mpf_class & get();
};
#define AS_FLT( x ) static_cast< var_flt_t * >( x )

class var_bool_t : public var_base_t
{
	bool m_val;
public:
	var_bool_t( const int val, const int parse_ctr = 0 );
	var_bool_t( const float val, const int parse_ctr = 0 );
	var_bool_t( const std::string & val, const int parse_ctr = 0 );
	var_bool_t( const bool val, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void assn( var_base_t * b );
	bool & get();
};
#define AS_BOOL( x ) static_cast< var_bool_t * >( x )

class var_enum_t : public var_base_t
{
	std::string m_name;
	std::unordered_map< std::string, var_int_t * > m_val;
public:
	var_enum_t( const std::string & name, std::unordered_map< std::string, var_int_t * > & val,
		    const int parse_ctr );
	~var_enum_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	std::string get_name();
	std::unordered_map< std::string, var_int_t * > & get_val();
};
#define AS_ENUM( x ) static_cast< var_enum_t * >( x )

class var_vec_t : public var_base_t
{
	std::vector< var_base_t * > m_val;
public:
	var_vec_t( std::vector< var_base_t * > & val, const int parse_ctr = 0 );
	~var_vec_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void clear();
	std::vector< var_base_t * > & get();
	void assn( var_base_t * b );
};
#define AS_VEC( x ) static_cast< var_vec_t * >( x )

class var_map_t : public var_base_t
{
	std::unordered_map< std::string, var_base_t * > m_val;
public:
	var_map_t( std::unordered_map< std::string, var_base_t * > & val, const int parse_ctr = 0 );
	~var_map_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void clear();
	std::unordered_map< std::string, var_base_t * > & get();
	void assn( var_base_t * b );
};
#define AS_MAP( x ) static_cast< var_map_t * >( x )

class var_tuple_t : public var_base_t
{
	std::vector< var_base_t * > m_val;
public:
	var_tuple_t( std::vector< var_base_t * > & val, const int parse_ctr = 0 );
	~var_tuple_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void clear();
	std::vector< var_base_t * > & get();
	void assn( var_base_t * b );
};
#define AS_TUPLE( x ) static_cast< var_tuple_t * >( x )

class var_struct_def_t : public var_base_t
{
	std::string m_name;
	std::vector< std::string > m_pos;
	std::unordered_map< std::string, var_base_t * > m_val;
public:
	var_struct_def_t( const std::string & name, std::vector< std::string > & pos,
			  std::unordered_map< std::string, var_base_t * > & val, const int parse_ctr = 0 );
	~var_struct_def_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	std::string & get_name();
	std::vector< std::string > & get_pos();
	std::unordered_map< std::string, var_base_t * > & get_val();
};

class var_struct_t : public var_base_t
{
	std::string m_name;
	std::unordered_map< std::string, var_base_t * > m_val;
public:
	var_struct_t( const std::string & name, std::unordered_map< std::string, var_base_t * > & val, const int parse_ctr = 0 );
	~var_struct_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int parse_ctr );
	void clear();
	std::string & get_name();
	std::unordered_map< std::string, var_base_t * > & get_val();
	void assn( var_base_t * b );
};
#define AS_STRUCT( x ) static_cast< var_struct_t * >( x )

#endif // VM_VARS_BASE_HPP