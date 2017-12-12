#include <math.h>
#include "correlfft.h"
#include "fft/autocross.h"

namespace namespace_correlation
{
COREDSP_API void fft_auto_correlation( REAL *f, ulong n, namespace_fft::fftProc *fft_func )
// compute (real, cyclic) self correlation of f[]
{
/*
	const ulong nh = ( n >> 1 );

	namespace_fft::RealFFt( f, n, -1, fft_func );
	Complex *fc = (Complex *)f;

	double v = 1. / (double)n;

	f[0] *= f[0] * v;
	f[1] *= f[1] * v;

	for ( ulong i = 1; i < nh; ++i )
	{
		fc[i].real( ( fc[i].real() * fc[i].real() + fc[i].imag() * fc[i].imag() ) * v );
		fc[i].imag( 0.0 );
		//		R0 += fc[i].real();
	}
	//	R0 *= 2.;
*/
	namespace_fft::AutoPowerSpectrumReal( f, n, fft_func );
	namespace_fft::RealFFt( f, n, 1, fft_func );
}

COREDSP_API void fft_correlation( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *fft_func )
// compute (real, cyclic) correlation of f[], g[]
{
/*
	ulong nh = n >> 1;
	Complex *fc = (Complex *)f;
	Complex *gc = (Complex *)g;

	namespace_fft::RealFFt( f, n, -1, fft_func );
	namespace_fft::RealFFt( g, n, -1, fft_func );

	double v = 1.0 / (double)n;

	g[0] *= f[0] * v;
	g[1] *= f[1] * v;

	for ( ulong i = 1; i < nh; ++i )
	{
		double gr = gc[i].real();
		double gi = gc[i].imag();
		gc[i].real( ( gr * fc[i].real() + gi * fc[i].imag() ) * v );
		gc[i].imag( ( gi * fc[i].real() - gr * fc[i].imag() ) * v );
	}
*/
	namespace_fft::CrossPowerSpectrumReal( f, g, n, fft_func );
	namespace_fft::RealFFt( g, n, 1, fft_func );
}

COREDSP_API void fft_complex_auto_correlation( Complex *f, ulong n, namespace_fft::fftProc *fft_func )
//
// complex (cyclic) self correlation
// (use zero padded data for linear correlation)
//
{
/*
	fft_func( f, n, 1 );

	const ulong nh = ( n >> 1 );

	double tr = f[0].real();
	double ti = f[0].imag();
	f[0].real( tr * tr - ti * ti );
	f[0].imag( ti * tr + ti * tr );
	
	tr = f[nh].real();
	ti = f[nh].imag();
	f[nh].real( tr * tr - ti * ti );
	f[nh].imag( ti * tr + ti * tr );


	double v = 1.0 / (double)n;
	for ( ulong k = 1, j = n - 1; k < j; k++, j-- )
	{
		double r1 = f[k].real();
		double i1 = f[k].imag();
		double r2 = f[j].real();
		double i2 = f[j].imag();

		f[k].real( r2 * r1 - i2 * i1 );
		f[k].imag( i2 * r1 + r2 * i1 );

		f[j].real( f[k].real() * v );
		f[j].imag( f[k].imag() * v );
	}
*/
	namespace_fft::AutoPowerSpectrumComplex( f, n, fft_func );
	fft_func( f, n, -1 );
}


COREDSP_API void fft_complex_correlation( Complex* f, Complex* g, ulong n, namespace_fft::fftProc *fft_func )
//
// complex (cyclic) correlation of fr,fi with gr,gi
// (use zero padded data for linear convolution)
//
{
/*
	fft_func( f, n, -1 );
	fft_func( g, n, 1 );

	double v = 1.0 / (double)n;

	for ( ulong k = 0; k < n; ++k )
	{
		double tr = f[k].real();
		double ti = f[k].imag();

		double t = tr * g[k].imag() + ti * g[k].real();
		tr *= g[k].real();
		tr -= ti * g[k].imag();
		ti = t;

		g[k].real( tr * v );
		g[k].imag( ti * v );
	}
*/
	namespace_fft::CrossPowerSpectrumComplex( f, g, n, fft_func );
	fft_func( g, n, -1 ); 
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestCorrelation()
{
	return false;
}
#endif


} //namespace namespace_correlation
