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
	VT_INT,
	VT_STR,
	VT_FLT,
	VT_BOOL,

	VT_ENUM,

	VT_FUNC,

	_VT_LAST,
};

extern const char * VarTypeStrs[ _VT_LAST ];

class var_base_t
{
	VarType m_type;
	int m_ref_ctr;
	const int m_parse_ctr;
public:
	var_base_t( const VarType type, const int parse_ctr );
	virtual ~var_base_t();

	VarType type() const;
	int ref() const;
	int parse_ctr() const;

	inline void inc_ref() { ++m_ref_ctr; }
	inline void dec_ref() { --m_ref_ctr; }

	virtual std::string type_str() const;
	virtual std::string to_str() const = 0;
	virtual mpz_class to_int() const = 0;
	virtual bool to_bool() const = 0;
	virtual var_base_t * copy() const = 0;
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

class var_int_t : public var_base_t
{
	mpz_class m_val;
public:
	var_int_t( const int val, const int parse_ctr );
	var_int_t( const std::string & val, const int parse_ctr );
	var_int_t( const bool val, const int parse_ctr );
	var_int_t( const float val, const int parse_ctr );
	var_int_t( const mpz_class & val, const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
	mpz_class & get();
};
#define AS_INT( x ) static_cast< var_int_t * >( x )

class var_str_t : public var_base_t
{
	std::string m_val;
public:
	var_str_t( const std::string & val, const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
	std::string & get();
};
#define AS_STR( x ) static_cast< var_str_t * >( x )

class var_flt_t : public var_base_t
{
	mpf_class m_val;
public:
	var_flt_t( const float val, const int parse_ctr );
	var_flt_t( const int val, const int parse_ctr );
	var_flt_t( const std::string & val, const int parse_ctr );
	var_flt_t( const bool val, const int parse_ctr );
	var_flt_t( const mpf_class & val, const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
	mpf_class & get();
};
#define AS_FLT( x ) static_cast< var_flt_t * >( x )

class var_bool_t : public var_base_t
{
	bool m_val;
public:
	var_bool_t( const int val, const int parse_ctr );
	var_bool_t( const float val, const int parse_ctr );
	var_bool_t( const std::string & val, const int parse_ctr );
	var_bool_t( const bool val, const int parse_ctr );
	var_bool_t( const mpf_class & val, const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy() const;
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
	var_base_t * copy() const;
	std::string get_name();
	std::unordered_map< std::string, var_int_t * > & get_val();
};
#define AS_ENUM( x ) static_cast< var_enum_t * >( x )

#endif // VM_VARS_BASE_HPP