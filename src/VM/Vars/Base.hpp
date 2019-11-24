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

#include "../../../third_party/mpfrxx.hpp"

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

#define DEFAULT_FLOAT_PRECISION 20

void update_float_precision( const int precision = DEFAULT_FLOAT_PRECISION );
int get_float_precision();

struct var_info_t
{
	int ref_ctr;
	int src_idx;
	int parse_ctr;
	bool implements_assign;
	var_info_t( const int rc, const int si ,const int pc, const bool ia );
};

class var_base_t
{
	VarType m_type;
	var_info_t m_info;
public:
	var_base_t( const VarType type, const bool implements_assign, const int src_idx, const int parse_ctr );
	virtual ~var_base_t();

	inline VarType type() const { return m_type; }
	inline int ref() const { return m_info.ref_ctr; }

	inline void set_parse_ctr( const int parse_ctr ) { m_info.parse_ctr = parse_ctr; }
	inline int parse_ctr() const { return m_info.parse_ctr; }

	inline void set_src_idx( const int src_idx ) { m_info.src_idx = src_idx; }
	inline int src_idx() const { return m_info.src_idx; }

	inline void inc_ref() { ++m_info.ref_ctr; }
	inline void dec_ref() { --m_info.ref_ctr; }
	inline bool impls_assn() { return m_info.implements_assign; }

	virtual std::string type_str() const;
	virtual std::string to_str() const = 0;
	virtual mpz_class to_int() const = 0;
	virtual bool to_bool() const = 0;
	virtual var_base_t * copy( const int src_idx, const int parse_ctr ) = 0;
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
	var_none_t( const int src_idx, const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
};
#define AS_NONE( x ) static_cast< var_none_t * >( x )

class var_nil_t : public var_base_t
{
public:
	var_nil_t( const int src_idx, const int parse_ctr );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
};
#define AS_NIL( x ) static_cast< var_nil_t * >( x )

class var_int_t : public var_base_t
{
	mpz_class m_val;
public:
	var_int_t( const int val, const int src_idx = 0, const int parse_ctr = 0 );
	var_int_t( const size_t val, const int src_idx = 0, const int parse_ctr = 0 );
	var_int_t( const std::string & val, const int src_idx = 0, const int parse_ctr = 0 );
	var_int_t( const bool val, const int src_idx = 0, const int parse_ctr = 0 );
	var_int_t( const float val, const int src_idx = 0, const int parse_ctr = 0 );
	var_int_t( const mpz_class & val, const int src_idx = 0, const int parse_ctr = 0 );
	var_int_t( const mpfr::mpreal & val, const int src_idx = 0, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	mpz_class & get();
};
#define AS_INT( x ) static_cast< var_int_t * >( x )

class var_str_t : public var_base_t
{
	std::string m_val;
public:
	var_str_t( const std::string & val, const int src_idx = 0, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	std::string & get();
};
#define AS_STR( x ) static_cast< var_str_t * >( x )

class var_flt_t : public var_base_t
{
	mpfr::mpreal m_val;
public:
	var_flt_t( const float val, const int src_idx = 0, const int parse_ctr = 0 );
	var_flt_t( const int val, const int src_idx = 0, const int parse_ctr = 0 );
	var_flt_t( const std::string & val, const int src_idx = 0, const int parse_ctr = 0 );
	var_flt_t( const bool val, const int src_idx = 0, const int parse_ctr = 0 );
	var_flt_t( const mpfr::mpreal & val, const int src_idx = 0, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	mpfr::mpreal & get();
};
#define AS_FLT( x ) static_cast< var_flt_t * >( x )

class var_bool_t : public var_base_t
{
	bool m_val;
public:
	var_bool_t( const int val, const int src_idx = 0, const int parse_ctr = 0 );
	var_bool_t( const float val, const int src_idx = 0, const int parse_ctr = 0 );
	var_bool_t( const std::string & val, const int src_idx = 0, const int parse_ctr = 0 );
	var_bool_t( const bool val, const int src_idx = 0, const int parse_ctr = 0 );

	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
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
		    const int src_idx = 0, const int parse_ctr = 0 );
	~var_enum_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	std::string get_name();
	std::unordered_map< std::string, var_int_t * > & get_val();
};
#define AS_ENUM( x ) static_cast< var_enum_t * >( x )

class var_vec_t : public var_base_t
{
	std::vector< var_base_t * > m_val;
	bool m_is_var_arg;
public:
	var_vec_t( const std::vector< var_base_t * > & val, const int src_idx = 0,
		   const int parse_ctr = 0, const bool is_var_arg = false );
	~var_vec_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void clear();
	std::vector< var_base_t * > & get();
	void assn( var_base_t * b );
	inline bool is_var_arg() const { return m_is_var_arg; }
	inline void set_var_arg( const bool is_var_arg ) { m_is_var_arg = is_var_arg; }
};
#define AS_VEC( x ) static_cast< var_vec_t * >( x )

class var_map_t : public var_base_t
{
	std::unordered_map< std::string, var_base_t * > m_val;
public:
	var_map_t( std::unordered_map< std::string, var_base_t * > & val,
		   const int src_idx = 0, const int parse_ctr = 0 );
	~var_map_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void clear();
	std::unordered_map< std::string, var_base_t * > & get();
	void assn( var_base_t * b );
};
#define AS_MAP( x ) static_cast< var_map_t * >( x )

class var_tuple_t : public var_base_t
{
	std::vector< var_base_t * > m_val;
public:
	var_tuple_t( const std::vector< var_base_t * > & val,
		     const int src_idx = 0, const int parse_ctr = 0 );
	~var_tuple_t();
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
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
			  std::unordered_map< std::string, var_base_t * > & val,
			  const int src_idx = 0, const int parse_ctr = 0 );
	~var_struct_def_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	std::string & get_name();
	std::vector< std::string > & get_pos();
	std::unordered_map< std::string, var_base_t * > & get_val();
};

class var_struct_t : public var_base_t
{
	std::string m_name;
	std::unordered_map< std::string, var_base_t * > m_val;
public:
	var_struct_t( const std::string & name, std::unordered_map< std::string, var_base_t * > & val,
		      const int src_idx = 0, const int parse_ctr = 0 );
	~var_struct_t();
	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void clear();
	std::string & get_name();
	std::unordered_map< std::string, var_base_t * > & get_val();
	void assn( var_base_t * b );
};
#define AS_STRUCT( x ) static_cast< var_struct_t * >( x )

#endif // VM_VARS_BASE_HPP