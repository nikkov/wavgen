#include <math.h>
#include "chirpz.h"
#include "dspexcept.h"
#include "arith.h"

namespace namespace_fft
{

extern void fft_dif4_core( Complex *f, ulong ldn );
extern void fft_dit4_core( Complex *f, ulong ldn );


#define  DIT_FFT_CORE  fft_dit4_core
#define  DIF_FFT_CORE  fft_dif4_core 

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

static void fft_complex_convolution( Complex *f, Complex *g, ulong ldn, double v = 0. )
//
// (complex, cyclic) convolution:  g[] :=  f[] (*) g[]
// (use zero padded data for usual convolution)
// ldn := base-2 logarithm of the array length
//
// Supply a value for v for a normalization factor != 1/n
//
{
	const ulong n = ( 1UL << ldn );

	DIF_FFT_CORE(f, ldn);
	DIF_FFT_CORE(g, ldn);
	if ( v == 0.0 )  
		v = 1.0/n;
	for ( ulong i = 0; i < n; ++i )
	{
		Complex t = g[i] * f[i];
		g[i] = t * (REAL)v;
	}
	DIT_FFT_CORE(g, ldn);
}

static void make_fft_chirp( Complex *w, ulong n, ulong nn, int is )
// for k=0..n-1:
//   w[k] = exp( is * k*k * (i*2*PI/n)/2 )
//   where i = sqrt(-1)
// for k=n..nn-1:
//   w[k] = 0
{
	double phi = 1.0 * is * M_PI / (double)n;
	ulong k2 = 0,  n2 = 2 * n;
	for ( ulong k = 0; k < n; ++k )
	{
		w[k] = SinCos( phi * k2 );
		k2 += ( 2 * k + 1);
		if ( k2 > n2 )  
			k2 -= n2;
		// here:  k2 == (k*k) mod 2*n;
	}
	null( w + n, nn - n );
}

static void make_fft_chirp_n2( Complex *w, ulong n, ulong nn, int is )
// for k=0..2*n-1:
//   w[k] = exp( is * k*k * (i*2*PI/n)/2 )
//   where i = sqrt(-1)
// for k=2*n..nn-1:
//   w[k] = 0
{
	double phi = 1.0 * is * M_PI / (double)n;
	ulong n2 = n * 2;
	ulong k2 = 0;
	for (ulong k=0; k<n2; ++k)
	{
		w[k] = SinCos( phi * k2 );
		k2 += ( 2 * k + 1 );
		if ( k2>n2 )  
			k2 -= n2;
		// here:  k2 == (k*k) mod 2*n;
	}
	null( w + n2, nn - n2 );
}


static void fft_arblen_even( Complex *x, ulong n, int is, DSPMemoryAllocator *alloc )
// Arbitrary length FFT for even lengths n.
{
	ulong ldnn = ld(n);
	if ( n == ( 1UL << ldnn ) )  
		ldnn += 1;
	else 
		ldnn += 2;
	ulong nn = ( 1UL << ldnn );

	Complex *f = alloc->Alloc<Complex>( nn );
	::copy0(x, n, f, nn);
	Complex *w = alloc->Alloc<Complex>( nn );

	make_fft_chirp( w, n, nn, is );
	multiply( f, n, w );

	double *dw = (double *)w;
	for ( ulong k = 1; k < 2 * n; k += 2 )
		dw[k] = -dw[k];

	fft_complex_convolution( w, f, ldnn );
	add( f, n, f + n );

	make_fft_chirp( w, n, nn, is );
	multiply( w, n, f );

	::copy( w, x, n );

	alloc->Free( w );
	alloc->Free( f );
}
// -------------------------


static void fft_arblen_odd( Complex *x, ulong n, int is, DSPMemoryAllocator *alloc )
// Arbitrary length FFT for odd lengths n.
{
	ulong ldnn = ld(n);
	ldnn += 3;
	ulong nn = ( 1UL << ldnn );

	Complex *f = alloc->Alloc<Complex>( nn );
	::copy0( x, n, f, nn );

	Complex *w = alloc->Alloc<Complex>( nn );

	make_fft_chirp_n2( w, n, nn, is );
	multiply( f, n, w );

	double *dw = (double *)w;
	for ( ulong k = 1; k < nn; k += 2 )  
		dw[k] = -dw[k];

	fft_complex_convolution( w, f, ldnn );
	ulong n2 = n * 2;
	ulong dn = nn - n2;
	if ( dn > n )
		dn = n;
	add( f, dn, f + n2 );  // need cyclic convolution

	make_fft_chirp( w, n, nn, is );
	multiply( w, n, f );

	::copy( w, x, n );

	alloc->Free( w );
	alloc->Free( f );
}


COREDSP_API void FftChirpZT( Complex *x, ulong n, int is, DSPMemoryAllocator *alloc )
// Arbitrary length FFT.
{
	if ( n & 1 )  
		fft_arblen_odd( x, n, is, alloc );
	else        
		fft_arblen_even( x, n, is, alloc );
}

COREDSP_API void FftChirpZT( Complex *x, ulong n, int is )
{
	FftChirpZT( x, n, is, defaultAllocator );
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestChirpZT()
{
	return false;
}
#endif

}//namespace namespace_fft

