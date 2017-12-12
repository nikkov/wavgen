#include <math.h>
#include "autocross.h"

namespace namespace_fft
{
COREDSP_API void CrossPowerSpectrumComplex( Complex *f, Complex *g, ulong n, namespace_fft::fftProc *fft_func )
{
	fft_func( f, n, -1 );
	fft_func( g, n, 1 );

	for ( ulong k = 0; k < n; ++k )
	{
		double tr = f[k].real();
		double ti = f[k].imag();

		double t = tr * g[k].imag() + ti * g[k].real();
		tr *= g[k].real();
		tr -= ti * g[k].imag();
		ti = t;

		g[k].real( tr );
		g[k].imag( ti );
	}
}

COREDSP_API void AutoPowerSpectrumComplex( Complex *f, ulong n, namespace_fft::fftProc *fft_func )
{
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

	for ( ulong k = 1, j = n - 1; k < j; k++, j-- )
	{
		double r1 = f[k].real();
		double i1 = f[k].imag();
		double r2 = f[j].real();
		double i2 = f[j].imag();

		f[k].real( r2 * r1 - i2 * i1 );
		f[k].imag( i2 * r1 + r2 * i1 );

		f[j].real( f[k].real() );
		f[j].imag( f[k].imag() );
	}
}

COREDSP_API void CrossPowerSpectrumReal( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *fft_func )
{
	ulong nh = n >> 1;
	Complex *fc = (Complex *)f;
	Complex *gc = (Complex *)g;

	namespace_fft::RealFFt( f, n, -1, fft_func );
	namespace_fft::RealFFt( g, n, -1, fft_func );

	g[0] *= f[0];
	g[1] *= f[1];

	for ( ulong i = 1; i < nh; ++i )
	{
		double gr = gc[i].real();
		double gi = gc[i].imag();
		gc[i].real( gr * fc[i].real() + gi * fc[i].imag() );
		gc[i].imag( gi * fc[i].real() - gr * fc[i].imag() );
	}
}

COREDSP_API void AutoPowerSpectrumReal( REAL *f, ulong n, namespace_fft::fftProc *fft_func )
{
	const ulong nh = ( n >> 1 );

	namespace_fft::RealFFt( f, n, -1, fft_func );
	Complex *fc = (Complex *)f;

	f[0] *= f[0];
	f[1] *= f[1];

	for ( ulong i = 1; i < nh; ++i )
	{
		fc[i].real( fc[i].real() * fc[i].real() + fc[i].imag() * fc[i].imag() );
		fc[i].imag( 0.0 );
	}
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestAutoCrossPowerSpectrum()
{
	return false;
}
#endif


} //namespace namespace_fft