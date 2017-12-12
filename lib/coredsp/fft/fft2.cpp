#include <math.h>
#include "fft2.h"
#include "dspexcept.h"

namespace namespace_fft
{
COREDSP_API void FftRadix2( Complex* f, ulong size, int direct )
{
	if ( !IsFftRadix2Length( size ) )
		THROW_EXCEPTION2( "Length %u isn't valid", size );

	ulong i, j, m, mmax, istep;
	double c, s, treal, timag, theta;

	/* compute transform */
	j = 1;
	for( i = 1; i <= size; i++ )
	{
		if( i < j )
		{
			treal = f[j-1].real();
			timag = f[j-1].imag();
			f[j-1].real( f[i-1].real() );
			f[j-1].imag( f[i-1].imag() );
			f[i-1].real( treal );
			f[i-1].imag( timag );
		}
		m = size >> 1;
		while( j > m )
		{
			j -= m;
			m = ( m + 1 ) >> 1;
		}
		j += m;
	}

	mmax = 1;
	while( size > mmax )
	{
		istep = mmax << 1;
		for( m = 1; m <= mmax; m++ )
		{
			theta =M_PI * (double)direct * (double)( m - 1 ) / (double)mmax;
			c = cos( theta );
			s = sin( theta );
			for( i = m; i <= size; i += istep )
			{
				j = i + mmax;
				treal = f[j-1].real() * c - f[j-1].imag() * s;
				timag = f[j-1].imag() * c + f[j-1].real() * s;

				f[j-1].real( f[i-1].real() - treal );
				f[j-1].imag( f[i-1].imag() - timag );
				f[i-1].real( f[i-1].real() + treal );
				f[i-1].imag( f[i-1].imag() + timag );
			}
		}
		mmax = istep;
	}
} 

COREDSP_API bool IsFftRadix2Length( ulong n )
{
	return ((n & -(long)n) == n);
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestFft2()
{
	return false;
}
#endif

}
