#include <math.h>
#include "hilbertfft.h"


namespace namespace_hilbert
{
COREDSP_API void fft_calc_analytic_signal( REAL *src, Complex *csrc, ulong len, namespace_fft::fftProc *func )
{
	ulong i;
	ulong lnh = len >> 1;
	Complex* cosrc = (Complex*)src;

	namespace_fft::RealFFt( src, len, -1, func );

	csrc[0].real( cosrc[0].real() );

	for( i = 1; i < lnh; i++ )
		csrc[i] = (REAL)2.*cosrc[i];

	for( i = lnh; i < len; i++ )
	{
		csrc[i].real( 0. );
		csrc[i].imag( 0. );
	}
	func( csrc, len, -1 );
	REAL v = (REAL)1./ (REAL)len;
	for( i = 0; i < len; i++ ) 
		csrc[i] *= v;
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestHilbert()
{
	return false;
}
#endif

}