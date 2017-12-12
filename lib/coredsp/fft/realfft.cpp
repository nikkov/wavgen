#include <math.h>
#include "export.h"
#include "realfft.h"
#include "pfafft.h"
#ifdef USE_TEST_PROC
#include "chirpz.h"
#include "fft2.h"
#include "mixfft.h"
#include "singltfft.h"
#endif
#include "dspexcept.h"

namespace namespace_fft
{

void real_complex_fft ( REAL *rz, Complex *cz, ulong n, int isign, fftProc* func )
{
	ulong i, ir, ii, jr, ji, no2;
	REAL *z;
	double tempr, tempi, sumr, sumi, difr, difi;
	double wr, wi, wpr, wpi, wtemp, theta;

	/* copy input to output while scaling */
	z = (REAL*)cz;
	for ( i = 0; i < n; i++ )
		z[i] = (REAL)0.5 * rz[i];

	/* do Complex to Complex transform */
	func( cz, n >> 1, isign );

	/* fix dc and nyquist */
	REAL z0 = z[0];
	z[0] = REAL(2. * ( z[0] + z[1] ) );
	z[1] = REAL(2. * ( z0 - z[1] ) );

	/* initialize cosine-sine recurrence */
	theta = 2. * M_PI / (double)n;
	if ( isign < 0 )
		theta = -theta;

	wtemp = sin( 0.5 * theta );
	wpr = -2. * wtemp * wtemp;
	wpi = sin(theta);
	wr = 1. + wpr;
	wi = wpi;

	/* twiddle */
	no2 = n >> 1;
	for ( ir = 2, ii = 3, jr = n - 2, ji = n - 1;
		ir <= no2;
		ir += 2, ii += 2, jr -= 2, ji -= 2 )
	{
		sumr = z[ir] + z[jr];
		sumi = z[ii] + z[ji];
		difr = z[ir] - z[jr];
		difi = z[ii] - z[ji];
		tempr = wi *difr + wr * sumi;
		tempi = wi *sumi - wr * difr;
		z[ir] = REAL( sumr + tempr);
		z[ii] = REAL( difi + tempi);
		z[jr] = REAL( sumr - tempr);
		z[ji] = REAL( tempi - difi);
		wtemp = wr;
		wr += wr * wpr - wi * wpi;
		wi += wi * wpr + wtemp * wpi;
	}
}

void complex_real_fft ( Complex *cz, REAL *rz, ulong n, int isign, fftProc* func )
{
	ulong i, ir, ii, jr, ji, no2;
	REAL *z;
	double tempr, tempi, sumr, sumi, difr, difi;
	double wr, wi, wpr, wpi, wtemp, theta;

	/* copy input to output and fix dc and nyquist */
	z = (REAL*)cz;
	for ( i = 2; i < n; i++ )
		rz[i] = z[i];

	REAL z0 = z[0];
	z[0] = REAL( z[0] + z[1] );
	z[1] = REAL( z0 - z[1]  );

	z = rz;

	/* initialize cosine-sine recurrence */
	theta = 2. * M_PI / (double)n;
	if ( isign > 0 )
		theta = -theta;
	wtemp = sin( 0.5 * theta );
	wpr = -2. * wtemp * wtemp;
	wpi = sin( theta );
	wr = 1. + wpr;
	wi = wpi;

	/* twiddle */
	no2 = n >> 1;
	for ( ir = 2, ii = 3, jr = n - 2, ji = n - 1;
		ir <= no2; ir += 2, ii += 2, jr -= 2, ji -= 2 )
	{
		sumr = z[ir] + z[jr];
		sumi = z[ii] + z[ji];
		difr = z[ir] - z[jr];
		difi = z[ii] - z[ji];
		tempr = wi * difr - wr * sumi;
		tempi = wi * sumi + wr * difr;
		z[ir] = REAL(sumr + tempr);
		z[ii] = REAL(difi + tempi);
		z[jr] = REAL(sumr - tempr);
		z[ji] = REAL(tempi - difi);
		wtemp = wr;
		wr += wr * wpr - wi * wpi;
		wi += wi * wpr + wtemp * wpi;
	}
	/* do Complex to Complex transform */
	func( (Complex*)z, n >> 1, isign );
}


COREDSP_API void RealFFt( REAL *x, ulong n, int isign, fftProc* func )
{
	if ( n & 1 )
		THROW_EXCEPTION( "Length for real fft must be even" );

	if( !func )
		func = PfaFft;
	if( isign == -1 )
		real_complex_fft( x, (Complex*)x, n, -1, func );
	else
		complex_real_fft( (Complex*)x, x, n, 1, func );

}

#ifdef USE_TEST_PROC

#define FFT_PREC	0.001

//from mathlab
REAL a[] = { 1., 2., 3., 4., 6., 4., 3., 4. };
REAL r[] = { 27., 0., 
			-6.4142, 1.4142,	
			1., 2., 
			-3.5858, 1.4142, 
			-1., 0. };


bool TestRealFft( fftProc* func )
{
	const ulong alen = sizeof(a)/sizeof(REAL);
	REAL x[alen];
	memcpy(x, a, sizeof(a));
	RealFFt( x, alen, -1, func );
	if( fabs(r[0] - x[0] ) > FFT_PREC )
		return false;
	if( fabs(r[8] - x[1] ) > FFT_PREC )
		return false;

	for( ulong i = 2; i < alen; i++ )
		if( fabs(r[i] - x[i] ) > FFT_PREC )
			return false;
	return true;
}


COREDSP_API bool TestRealFft()
{
	return TestRealFft(PfaFft) && TestRealFft(FftChirpZT) && TestRealFft(FftRadix2) &&
		TestRealFft(FftDit4) && TestRealFft(SingFFTStatic) && TestRealFft(SingFFTDynamic);
}
#endif

} //namespace_fft
