#include <math.h>
#include "convolfft.h"
#include "fft/arith.h"

namespace namespace_convolution
{
COREDSP_API void fft_complex_auto_convolution( Complex *f, ulong n, double v, namespace_fft::fftProc *func )
//
// (complex, cyclic) self convolution:  f[] :=  f[] (*) f[]
// (use zero padded data for linear convolution)
//
// ldn := base-2 logarithm of the array length
// Supply a value for v for a normalization factor != 1/n
{
	func( f, n, -1 );
	if ( v == 0.0 )  
		v = 1.0 / (double)n;
	for ( ulong i = 0; i < n; ++i )
	{
		Complex t = f[i];
		t *= t;
		f[i] = t * (REAL)v;
	}
	func( f, n, 1 );
}


COREDSP_API void fft_complex_convolution( Complex *f, Complex *g, ulong n, double v, namespace_fft::fftProc *func )
//
// (complex, cyclic) convolution:  g[] :=  f[] (*) g[]
// (use zero padded data for usual convolution)
// ldn := base-2 logarithm of the array length
//
// Supply a value for v for a normalization factor != 1/n
//
{
	func( f, n, -1 );
	func( g, n, -1 );
	if ( v == 0.0 )
		v = 1.0 / (double)n;
	for (ulong i=0; i<n; ++i)
	{
		Complex t = g[i] * f[i];
		g[i] = t * (REAL)v;
	}
	func( g, n, 1 );
}


static inline void fft_auto_convolution_core( REAL *f, ulong n, double v )
// auxiliary routine for FFT based (real) self convolutions
// supply a value for v for a normalization factor != 1/n
{
	if ( v == 0.0 )  
		v = 1.0 / (double)n;

	f[0]  *= f[0] * v;

	const ulong nh = n >> 1;
	if ( nh > 0 )
	{
		f[nh] *= f[nh] * v;
		for ( ulong i = 1, j = n - 1;  i < j;  ++i, --j )
		{
			csqr_n( f[i], f[j], v );
		}
	}
}

COREDSP_API void fft_auto_convolution( REAL *f, ulong n, namespace_fft::fftProc *func )
// (cyclic, real) self convolution:  f[] :=  f[] (*) f[]
// ldn := base-2 logarithm of the array length
{
	namespace_fft::RealFFt( f, n, -1, func );
	fft_auto_convolution_core( f, n, 0. );
	namespace_fft::RealFFt( f, n, -1, func );
}

static inline void fft_convolution_core(const REAL *f, REAL *g, ulong n, double v )
// Auxiliary routine for FFT based convolutions.
// Supply a value for v for a normalization factor != 1/n
{
	if ( v == 0.0 )
		v = 1.0 / (double)n;

	g[0]  *= f[0] * v;
	const ulong nh = n >> 1;
	g[nh] *= f[nh] * v;

	for ( ulong i = 1, j = n - 1; i < j; ++i, --j )
	{
		cmult_n( f[i], f[j], g[i], g[j], v );
	}
} 

COREDSP_API void fft_convolution( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *func )
// (cyclic, real) convolution:  g[] :=  f[] (*) g[]
// ldn := base-2 logarithm of the array length
{
	namespace_fft::RealFFt( f, n, -1, func );
	namespace_fft::RealFFt( g, n, -1, func );
	fft_convolution_core( f, g, n, 0. );
	namespace_fft::RealFFt( g, n, -1, func );
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestConvolution()
{
	return false;
}
#endif

}//namespace namespace_convolution