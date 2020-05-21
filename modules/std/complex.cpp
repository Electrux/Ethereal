/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../third_party/mpcxx.hpp"

#include "../../src/VM/Core.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Class ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class var_cplx_t : public var_base_t
{
	mpc::mpcplx m_val;
public:
	var_cplx_t( const mpc::mpcplx & val, const int src_idx = 0, const int parse_ctr = 0 );
	~var_cplx_t();

	std::string type_str() const;
	std::string to_str() const;
	mpz_class to_int() const;
	bool to_bool() const;
	var_base_t * copy( const int src_idx, const int parse_ctr );
	void assn( var_base_t * b );
	mpc::mpcplx & get();
};
#define AS_CPLX( x ) static_cast< var_cplx_t * >( x )

var_cplx_t::var_cplx_t( const mpc::mpcplx & val, const int src_idx, const int parse_ctr )
	: var_base_t( VT_CUSTOM, true, src_idx, parse_ctr ), m_val( val ) {}
var_cplx_t::~var_cplx_t() {}

std::string var_cplx_t::type_str() const { return "cplx_t"; }
std::string var_cplx_t::to_str() const
{
	return "cplx_t{" + m_val.to_str( get_float_precision() ) + "}";
}
mpz_class var_cplx_t::to_int() const { return mpz_class( mpf_class( m_val.real().toString() ) ); }
bool var_cplx_t::to_bool() const { return m_val.real() != 0.0 && m_val.imag() != 0.0; }
var_base_t * var_cplx_t::copy( const int src_idx, const int parse_ctr )
{
	return new var_cplx_t( m_val, src_idx, parse_ctr );
}
void var_cplx_t::assn( var_base_t * b )
{
	m_val = AS_CPLX( b )->get();
}
mpc::mpcplx & var_cplx_t::get() { return m_val; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// Functions //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DECL_FUNC_ALLOC__CPLX( name, op )									\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )						\
	{													\
		return new var_cplx_t( AS_CPLX( fcd.args[ 1 ] )->get() op AS_CPLX( fcd.args[ 0 ] )->get() );	\
	}													\

#define DECL_FUNC_ASSN__CPLX( name, op )									\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )						\
	{													\
		auto & lhs = AS_CPLX( fcd.args[ 0 ] )->get();							\
		auto & rhs = AS_CPLX( fcd.args[ 1 ] )->get();							\
		lhs op rhs;											\
		return fcd.args[ 0 ];										\
	}													\

#define DECL_FUNC_BOOL__CPLX( name, op )									\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )						\
	{													\
		return TRUE_FALSE( AS_CPLX( fcd.args[ 1 ] )->get() op AS_CPLX( fcd.args[ 0 ] )->get() );	\
	}													\

var_base_t * cplx_create_z( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & real = AS_INT( fcd.args[ 1 ] )->get();
	mpz_class imag   = fcd.args.size() > 2 ? AS_INT( fcd.args[ 2 ] )->get() : 0;
	return new var_cplx_t( mpc::mpcplx( real, imag ) );
}

var_base_t * cplx_create_f( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & real = AS_FLT( fcd.args[ 1 ] )->get();
	mpfr::mpreal imag   = fcd.args.size() > 2 ? AS_FLT( fcd.args[ 2 ] )->get() : 0.0;
	return new var_cplx_t( mpc::mpcplx( real, imag ) );
}

DECL_FUNC_ALLOC__CPLX( cplx_add, + )
DECL_FUNC_ALLOC__CPLX( cplx_sub, - )
DECL_FUNC_ALLOC__CPLX( cplx_mul, * )
DECL_FUNC_ALLOC__CPLX( cplx_div, / )

DECL_FUNC_ASSN__CPLX( cplx_add_assn, += )
DECL_FUNC_ASSN__CPLX( cplx_sub_assn, -= )
DECL_FUNC_ASSN__CPLX( cplx_mul_assn, *= )
DECL_FUNC_ASSN__CPLX( cplx_div_assn, /= )

DECL_FUNC_BOOL__CPLX( cplx_eq, == )
DECL_FUNC_BOOL__CPLX( cplx_ne, != )

var_base_t * cplx_real( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_flt_t( AS_CPLX( fcd.args[ 0 ] )->get().real() );
}

var_base_t * cplx_imag( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_flt_t( AS_CPLX( fcd.args[ 0 ] )->get().imag() );
}

var_base_t * cplx_abs( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_flt_t( AS_CPLX( fcd.args[ 0 ] )->get().abs() );
}

var_base_t * cplx_arg( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_flt_t( AS_CPLX( fcd.args[ 0 ] )->get().arg() );
}

var_base_t * cplx_norm( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_flt_t( AS_CPLX( fcd.args[ 0 ] )->get().norm() );
}

var_base_t * cplx_conj( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().conj() );
}

var_base_t * cplx_proj( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().proj() );
}

var_base_t * cplx_uminus( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( -( AS_CPLX( fcd.args[ 0 ] )->get() ) );
}

var_base_t * cplx_exp( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().exp() );
}

var_base_t * cplx_log( vm_state_t & vm, func_call_data_t & fcd )
{
	mpc::mpcplx & num = AS_CPLX( fcd.args[ 0 ] )->get();
	mpc::mpcplx base = fcd.args.size() > 1 ? AS_CPLX( fcd.args[ 1 ] )->get() : mpc::mpcplx( mpz_class( 10 ) );
	return new var_cplx_t( num.log() / base.log() );
}

var_base_t * cplx_pow( vm_state_t & vm, func_call_data_t & fcd )
{
	mpc::mpcplx & num = AS_CPLX( fcd.args[ 1 ] )->get();
	mpc::mpcplx & exp = AS_CPLX( fcd.args[ 0 ] )->get();
	return new var_cplx_t( num.pow( exp ) );
}

var_base_t * cplx_pow_z( vm_state_t & vm, func_call_data_t & fcd )
{
	mpc::mpcplx & num = AS_CPLX( fcd.args[ 1 ] )->get();
	mpz_class & exp = AS_INT( fcd.args[ 0 ] )->get();
	return new var_cplx_t( num.pow( exp ) );
}

var_base_t * cplx_pow_f( vm_state_t & vm, func_call_data_t & fcd )
{
	mpc::mpcplx & num = AS_CPLX( fcd.args[ 1 ] )->get();
	mpfr::mpreal & exp = AS_FLT( fcd.args[ 0 ] )->get();
	return new var_cplx_t( num.pow( exp ) );
}

var_base_t * cplx_sqrt( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().sqrt() );
}

var_base_t * cplx_sin( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().sin() );
}

var_base_t * cplx_cos( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().cos() );
}

var_base_t * cplx_tan( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().tan() );
}

var_base_t * cplx_csc( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().csc() );
}

var_base_t * cplx_sec( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().sec() );
}

var_base_t * cplx_cot( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_cplx_t( AS_CPLX( fcd.args[ 0 ] )->get().cot() );
}

REGISTER_MODULE( complex )
{
	functions_t & cplxt = vm.typefuncs[ "_cplx_t" ];
	cplxt.add( { "new", 1, 2, { "int", "int" }, FnType::MODULE, { .modfn = cplx_create_z }, true } );
	cplxt.add( { "new", 1, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = cplx_create_f }, true } );

	vm.funcs.add( { "+",  2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_add }, true } );
	vm.funcs.add( { "-",  2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_sub }, true } );
	vm.funcs.add( { "*",  2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_mul }, true } );
	vm.funcs.add( { "/",  2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_div }, true } );

	vm.funcs.add( { "+=", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_add_assn }, false } );
	vm.funcs.add( { "-=", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_sub_assn }, false } );
	vm.funcs.add( { "*=", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_mul_assn }, false } );
	vm.funcs.add( { "/=", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_div_assn }, false } );

	vm.funcs.add( { "**", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_pow }, true } );
	vm.funcs.add( { "**", 2, 2, { "int", "cplx_t" }, FnType::MODULE, { .modfn = cplx_pow_z }, true } );
	vm.funcs.add( { "**", 2, 2, { "flt", "cplx_t" }, FnType::MODULE, { .modfn = cplx_pow_f }, true } );

	vm.funcs.add( { "==", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_eq }, false } );
	vm.funcs.add( { "!=", 2, 2, { "cplx_t", "cplx_t" }, FnType::MODULE, { .modfn = cplx_ne }, false } );

	vm.funcs.add( { "u-", 1, 1, { "cplx_t" }, FnType::MODULE, { .modfn = cplx_uminus }, true } );

	functions_t & cplx = vm.typefuncs[ "cplx_t" ];
	cplx.add( { "real", 0, 0, {}, FnType::MODULE, { .modfn = cplx_real }, true } );
	cplx.add( { "imag", 0, 0, {}, FnType::MODULE, { .modfn = cplx_imag }, true } );
	cplx.add( { "abs",  0, 0, {}, FnType::MODULE, { .modfn = cplx_abs }, true } );
	cplx.add( { "arg",  0, 0, {}, FnType::MODULE, { .modfn = cplx_arg }, true } );
	cplx.add( { "norm", 0, 0, {}, FnType::MODULE, { .modfn = cplx_norm }, true } );
	cplx.add( { "conj", 0, 0, {}, FnType::MODULE, { .modfn = cplx_conj }, true } );
	cplx.add( { "proj", 0, 0, {}, FnType::MODULE, { .modfn = cplx_proj }, true } );
	cplx.add( { "exp",  0, 0, {}, FnType::MODULE, { .modfn = cplx_exp }, true } );
	cplx.add( { "log",  0, 1, { "cplx_t" }, FnType::MODULE, { .modfn = cplx_log }, true } );
	// pow functions above as '**'
	cplx.add( { "sqrt", 0, 0, {}, FnType::MODULE, { .modfn = cplx_sqrt }, true } );
	cplx.add( { "sin",  0, 0, {}, FnType::MODULE, { .modfn = cplx_sin }, true } );
	cplx.add( { "cos",  0, 0, {}, FnType::MODULE, { .modfn = cplx_cos }, true } );
	cplx.add( { "tan",  0, 0, {}, FnType::MODULE, { .modfn = cplx_tan }, true } );
	cplx.add( { "csc",  0, 0, {}, FnType::MODULE, { .modfn = cplx_csc }, true } );
	cplx.add( { "sec",  0, 0, {}, FnType::MODULE, { .modfn = cplx_sec }, true } );
	cplx.add( { "cot",  0, 0, {}, FnType::MODULE, { .modfn = cplx_cot }, true } );
}