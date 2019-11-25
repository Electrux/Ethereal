#ifndef MPCXX_HPP
#define MPCXX_HPP

#include <gmpxx.h>
#include <mpc.h>

#include "mpfrxx.hpp"

namespace mpc
{
class mpcplx
{
	mpc_t m_val;
public:

	inline static mpc_rnd_t get_default_rnd() { return MPC_RNDNN; }
	inline static mp_prec_t get_default_prec() { return mpfr_get_default_prec(); }

	// destructor
	~mpcplx();

	// constructors
	mpcplx();
	mpcplx( const mpcplx & v );
	mpcplx( const mpz_class & v );
	mpcplx( const mpfr::mpreal & v );
	mpcplx( const mpz_class & v, const mpz_class & i );
	mpcplx( const mpfr::mpreal & v, const mpfr::mpreal & i );

	mpcplx & operator =( mpcplx v );

	// unary
	mpcplx operator -();

	// arithmetic
	friend mpcplx operator +( const mpcplx & a, const mpcplx & b );
	friend mpcplx operator +( const mpcplx & a, const mpfr::mpreal & b );
	friend mpcplx operator +( const mpfr::mpreal & a, const mpcplx & b );

	friend mpcplx operator -( const mpcplx & a, const mpcplx & b );
	friend mpcplx operator -( const mpcplx & a, const mpfr::mpreal & b );
	friend mpcplx operator -( const mpfr::mpreal & a, const mpcplx & b );

	friend mpcplx operator *( const mpcplx & a, const mpcplx & b );
	friend mpcplx operator *( const mpcplx & a, const mpfr::mpreal & b );
	friend mpcplx operator *( const mpfr::mpreal & a, const mpcplx & b );

	friend mpcplx operator /( const mpcplx & a, const mpcplx & b );
	friend mpcplx operator /( const mpcplx & a, const mpfr::mpreal & b );
	friend mpcplx operator /( const mpfr::mpreal & a, const mpcplx & b );

	// arithmetic assign
	mpcplx & operator +=( const mpcplx & v );
	mpcplx & operator +=( const mpfr::mpreal & v );

	mpcplx & operator -=( const mpcplx & v );
	mpcplx & operator -=( const mpfr::mpreal & v );

	mpcplx & operator *=( const mpcplx & v );
	mpcplx & operator *=( const mpfr::mpreal & v );

	mpcplx & operator /=( const mpcplx & v );
	mpcplx & operator /=( const mpfr::mpreal & v );

	bool operator ==( const mpcplx & v ) const;
	bool operator !=( const mpcplx & v ) const;

	mpfr::mpreal real() const;
	mpfr::mpreal imag() const;

	mpfr::mpreal abs() const;
	mpfr::mpreal arg() const;
	mpfr::mpreal norm() const;
	mpcplx conj() const;
	mpcplx proj() const;

	// log + power
	mpcplx exp() const;
	mpcplx log() const;
	mpcplx pow( const mpcplx & base ) const;
	mpcplx pow( const mpz_class & base ) const;
	mpcplx pow( const mpfr::mpreal & base ) const;
	mpcplx sqrt() const;

	// trignometric
	mpcplx sin() const;
	mpcplx cos() const;
	mpcplx tan() const;

	mpcplx csc() const;
	mpcplx sec() const;
	mpcplx cot() const;

	std::string to_str( size_t prec = get_default_prec(), const int base = 10 ) const;
};

// destructor
inline mpcplx::~mpcplx()
{
	mpc_clear( m_val );
}

// constructors
inline mpcplx::mpcplx()
{
	mpc_init2( m_val, get_default_prec() );
}
inline mpcplx::mpcplx( const mpcplx & v )
{
	mpc_init2( m_val, get_default_prec() );
	mpc_set( m_val, v.m_val, get_default_rnd() );
}
inline mpcplx::mpcplx( const mpz_class & v )
{
	mpc_init2( m_val, get_default_prec() );
	mpc_set_z( m_val, v.get_mpz_t(), get_default_rnd() );
}
inline mpcplx::mpcplx( const mpz_class & v, const mpz_class & i )
{
	mpc_init2( m_val, get_default_prec() );
	mpc_set_z_z( m_val, v.get_mpz_t(), i.get_mpz_t(), get_default_rnd() );
}
inline mpcplx::mpcplx( const mpfr::mpreal & v )
{
	mpc_init2( m_val, get_default_prec() );
	mpc_set_fr( m_val, v.mpfr_ptr(), get_default_rnd() );
}
inline mpcplx::mpcplx( const mpfr::mpreal & v, const mpfr::mpreal & i )
{
	mpc_init2( m_val, get_default_prec() );
	mpc_set_fr_fr( m_val, v.mpfr_ptr(), i.mpfr_ptr(), get_default_rnd() );
}

// unary
inline mpcplx mpcplx::operator-()
{
	mpcplx r;
	mpc_neg( r.m_val, m_val, get_default_rnd() );
	return r;
}

// arithmetic
inline mpcplx operator +( const mpcplx & a, const mpcplx & b )
{
	mpcplx r;
	mpc_add( r.m_val, a.m_val, b.m_val, mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator +( const mpcplx & a, const mpfr::mpreal & b )
{
	mpcplx r;
	mpc_add_fr( r.m_val, a.m_val, b.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator +( const mpfr::mpreal & a, const mpcplx & b )
{
	mpcplx r;
	mpc_add_fr( r.m_val, b.m_val, a.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}

inline mpcplx operator -( const mpcplx & a, const mpcplx & b )
{
	mpcplx r;
	mpc_sub( r.m_val, a.m_val, b.m_val, mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator -( const mpcplx & a, const mpfr::mpreal & b )
{
	mpcplx r;
	mpc_sub_fr( r.m_val, a.m_val, b.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator -( const mpfr::mpreal & a, const mpcplx & b )
{
	mpcplx r;
	mpc_sub_fr( r.m_val, b.m_val, a.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}

inline mpcplx operator *( const mpcplx & a, const mpcplx & b )
{
	mpcplx r;
	mpc_mul( r.m_val, a.m_val, b.m_val, mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator *( const mpcplx & a, const mpfr::mpreal & b )
{
	mpcplx r;
	mpc_mul_fr( r.m_val, a.m_val, b.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator *( const mpfr::mpreal & a, const mpcplx & b )
{
	mpcplx r;
	mpc_mul_fr( r.m_val, b.m_val, a.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}

inline mpcplx operator /( const mpcplx & a, const mpcplx & b )
{
	mpcplx r;
	mpc_div( r.m_val, a.m_val, b.m_val, mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator /( const mpcplx & a, const mpfr::mpreal & b )
{
	mpcplx r;
	mpc_div_fr( r.m_val, a.m_val, b.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}
inline mpcplx operator /( const mpfr::mpreal & a, const mpcplx & b )
{
	mpcplx r;
	mpc_div_fr( r.m_val, b.m_val, a.mpfr_ptr(), mpcplx::get_default_rnd() );
	return r;
}

// copy assignment operator
inline mpcplx & mpcplx::operator =( mpcplx v )
{
	mpc_set( m_val, v.m_val, get_default_rnd() );
	return * this;
}

// arithmetic assign
inline mpcplx & mpcplx::operator +=( const mpcplx & v )
{
	mpc_add( m_val, m_val, v.m_val, get_default_rnd() );
	return * this;
}
inline mpcplx & mpcplx::operator +=( const mpfr::mpreal & v )
{
	mpc_add_fr( m_val, m_val, v.mpfr_ptr(), get_default_rnd() );
	return * this;
}

inline mpcplx & mpcplx::operator -=( const mpcplx & v )
{
	mpc_sub( m_val, m_val, v.m_val, get_default_rnd() );
	return * this;
}
inline mpcplx & mpcplx::operator -=( const mpfr::mpreal & v )
{
	mpc_sub_fr( m_val, m_val, v.mpfr_ptr(), get_default_rnd() );
	return * this;
}

inline mpcplx & mpcplx::operator *=( const mpcplx & v )
{
	mpc_mul( m_val, m_val, v.m_val, get_default_rnd() );
	return * this;
}
inline mpcplx & mpcplx::operator *=( const mpfr::mpreal & v )
{
	mpc_mul_fr( m_val, m_val, v.mpfr_ptr(), get_default_rnd() );
	return * this;
}

inline mpcplx & mpcplx::operator /=( const mpcplx & v )
{
	mpc_div( m_val, m_val, v.m_val, get_default_rnd() );
	return * this;
}
inline mpcplx & mpcplx::operator /=( const mpfr::mpreal & v )
{
	mpc_div_fr( m_val, m_val, v.mpfr_ptr(), get_default_rnd() );
	return * this;
}

// logical comparison
inline bool mpcplx::operator ==( const mpcplx & v ) const
{
	return mpc_cmp( m_val, v.m_val );
}
inline bool mpcplx::operator !=( const mpcplx & v ) const
{
	return !mpc_cmp( m_val, v.m_val );
}

// get real/imaginary
inline mpfr::mpreal mpcplx::real() const
{
	mpfr::mpreal r;
	mpc_real( r.mpfr_ptr(), m_val, mpfr::mpreal::get_default_rnd() );
	return r;
}
inline mpfr::mpreal mpcplx::imag() const
{
	mpfr::mpreal r;
	mpc_imag( r.mpfr_ptr(), m_val, mpfr::mpreal::get_default_rnd() );
	return r;
}

// misc
inline mpfr::mpreal mpcplx::abs() const
{
	mpfr::mpreal r;
	mpc_abs( r.mpfr_ptr(), m_val, mpfr::mpreal::get_default_rnd() );
	return r;
}
inline mpfr::mpreal mpcplx::arg() const
{
	mpfr::mpreal r;
	mpc_arg( r.mpfr_ptr(), m_val, mpfr::mpreal::get_default_rnd() );
	return r;
}
inline mpfr::mpreal mpcplx::norm() const
{
	mpfr::mpreal r;
	mpc_norm( r.mpfr_ptr(), m_val, mpfr::mpreal::get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::conj() const
{
	mpcplx r;
	mpc_conj( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::proj() const
{
	mpcplx r;
	mpc_proj( r.m_val, m_val, get_default_rnd() );
	return r;
}

// log + power
inline mpcplx mpcplx::exp() const
{
	mpcplx r;
	mpc_exp( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::log() const
{
	mpcplx r;
	mpc_log( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::pow( const mpcplx & base ) const
{
	mpcplx r;
	mpc_pow( r.m_val, m_val, base.m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::pow( const mpz_class & base ) const
{
	mpcplx r;
	mpc_pow_z( r.m_val, m_val, base.get_mpz_t(), get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::pow( const mpfr::mpreal & base ) const
{
	mpcplx r;
	mpc_pow_fr( r.m_val, m_val, base.mpfr_ptr(), get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::sqrt() const
{
	mpcplx r;
	mpc_sqrt( r.m_val, m_val, get_default_rnd() );
	return r;
}

// trignometric
inline mpcplx mpcplx::sin() const
{
	mpcplx r;
	mpc_sin( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::cos() const
{
	mpcplx r;
	mpc_cos( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::tan() const
{
	mpcplx r;
	mpc_tan( r.m_val, m_val, get_default_rnd() );
	return r;
}

inline mpcplx mpcplx::csc() const
{
	mpcplx r;
	mpc_asin( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::sec() const
{
	mpcplx r;
	mpc_acos( r.m_val, m_val, get_default_rnd() );
	return r;
}
inline mpcplx mpcplx::cot() const
{
	mpcplx r;
	mpc_atan( r.m_val, m_val, get_default_rnd() );
	return r;
}

inline std::string mpcplx::to_str( size_t prec, const int base ) const
{
	char * buf = mpc_get_str( base, prec, m_val, get_default_rnd() );
	std::string str( buf );
	mpc_free_str( buf );
	return str;
}

inline std::ostream & operator <<( std::ostream & os, const mpcplx & z )
{
    return os << z.to_str( static_cast< size_t >( os.precision() ) );
}

// end namespace
}

#endif // MPCXX_HPP
