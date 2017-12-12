#include <math.h>
#include "utilfunc.h"
#include "fft/singltfft.h"


namespace namespace_utilfunc
{

COREDSP_API void RemoveLineTrend( REAL *src, ulong len )
{
	double sxq = 0., sy = 0., sxy = 0., b1, b0;
	ulong isx = 0;

	REAL *pf = src;
	for(ulong i = 0; i < len; i++, pf++) 
	{
			isx += i; 
			sy += *pf;
			sxy += i * (*pf); 
			sxq += double(i) * i;
	}
	double sx = isx;
	double sx2 = sx * sx;
	double sxsy = sx * sy;
	double lbsxy = (double)len * sxy;
	double lbsxq = (double)len * sxq;
	b1 = ( sxsy - lbsxy ) / ( sx2 - lbsxq );
	double b1sx = b1 * sx;
	b0 = ( sy - b1sx ) / (double)len;
	pf = src;
	for(ulong i = 0; i < len; i++, pf++ )
		*pf -= (REAL)(b1 * (double)i + b0);
}

COREDSP_API void RemoveLineTrend( Complex *src, ulong len )
{
	double sxq = 0., sy = 0., sxy = 0., b1, b0, mag;
	ulong isx = 0;

	Complex *pf = src;
	for(ulong i = 0; i < len; i++, pf++) 
	{
		isx += i; 
		mag = abs( *pf );
		sy += mag;
		sxy += i * mag; 
		sxq += double( i * i );
	}
	double sx = isx;
	double sx2 = sx * sx;
	double sxsy = sx * sy;
	double lbsxy = (double)len * sxy;
	double lbsxq = (double)len * sxq;
	b1 = ( sxsy - lbsxy ) / ( sx2 - lbsxq );
	double b1sx = b1 * sx;
	b0 = ( sy - b1sx ) / (double)len;
	pf = src;
	for(ulong i = 0; i < len; i++, pf++ )
	{
		mag = b1 * (double)i + b0;
		pf->real( (REAL) (pf->real() - mag) );
		pf->imag( (REAL) (pf->imag() - mag) );
	}
}


COREDSP_API void fft_integral_real( REAL *src, ulong n, double freq, namespace_fft::fftProc *fft_func )
{
	namespace_fft::RealFFt( src, n, -1, fft_func );
	Complex *csrc = (Complex *)src;

	ulong zeros = 1;
	for( ulong i = 0; i < zeros; i++ )
	{
		csrc[i].real( 0. );
		csrc[i].imag( 0. );
	}

	ulong lnh = n >> 1;

	double dw = freq / (double)n * 2. * M_PI;
	double v = 1. / dw / (double)n;
	for( ulong i = zeros; i < lnh; i++ )
	{
		double kf = v / (double)i;
		csrc[i] *= (REAL)kf;

		//изменение фазы
		REAL tmp = csrc[i].imag();
		csrc[i].imag( -csrc[i].real() );
		csrc[i].real( tmp );
	}
	namespace_fft::RealFFt( src, n, 1, fft_func );
	RemoveLineTrend( src, n );
}

COREDSP_API void fft_integral_complex( Complex *src, ulong n, double freq, namespace_fft::fftProc *fft_func )
{
	fft_func( src, n, -1 );
	ulong zeros = 1;
	for( ulong i = 0; i < zeros; i++ )
	{
		src[i].real( 0. );
		src[i].imag( 0. );
	}

	double dw = freq / (double)n * 2. * M_PI;
	double v = 1. / dw / (double)n;
	for( ulong i = zeros; i < n; i++ )
	{
		double kf = v / (double)i;
		src[i] *= (REAL)kf;

		//изменение фазы
		REAL tmp = src[i].imag();
		src[i].imag( -src[i].real() );
		src[i].real( tmp );
	}
	fft_func( src, n, 1 );
	RemoveLineTrend( src, n );
}

COREDSP_API void fft_differen_real( REAL *src, ulong n, double freq, namespace_fft::fftProc *fft_func )
{
	namespace_fft::RealFFt( src, n, -1, fft_func );
	Complex *csrc = (Complex *)src;
	ulong lnh = n >> 1;
	double df = freq/(double)n;
	double v = 2. * M_PI * df;
	for( ulong i = 1; i < lnh; i++ )
		csrc[i] *= (REAL)((REAL)i * v);
	namespace_fft::RealFFt( src, n, 1, fft_func );
}

COREDSP_API void fft_differen_complex( Complex *src, ulong n, double freq, namespace_fft::fftProc *fft_func )
{
	fft_func( src, n, -1 );
	src[0].real( 0. );
	src[0].imag( 0. );
	double df = freq/(double)n;
	double v = 2. * M_PI * df;
	for( ulong i = 1; i < n; i++ )
		src[i] *= (REAL)((REAL)i * v);
	fft_func( src, n, 1 );
}


#ifdef USE_TEST_PROC

#define	INT_DIFF_PREC	0.1

bool TestIntegrReal()
{
	REAL a[1024];
	const ulong n = sizeof(a) / sizeof(REAL);
	REAL b[n];

	REAL f_descr = (REAL)16384.;
	REAL f = (REAL)1000. / (REAL)2. / (REAL)M_PI;
	for( ulong i = 0; i < n; i++ )
		a[i] = (REAL)1000.*sin( (REAL)2. * (REAL)M_PI * (REAL)i * f / f_descr);


	memcpy( b, a, sizeof(b) );
	fft_integral_real( b, n, f_descr, namespace_fft::SingFFTStatic );


//	FILE *logf = fopen("log.int", "wt");
	for(ulong i = 0; i < n; i++ )
	{
		double val = -1000.*cos( 2. * M_PI * (double)i * f / f_descr );
		double val1 = 1000. * b[i]; // переход от м/с2 к мм/с
//		fprintf( logf, "%f\t%f\n", val, val1 );
		if( fabs ( ( val1 - val ) / 1000. ) > INT_DIFF_PREC )
				return false;
	}
//	fclose(logf);
	return true;
}

bool TestIntegrComplex()
{
	Complex a[1024];
	const ulong n = sizeof(a) / sizeof(Complex);
	Complex b[n];

	REAL f_descr = (REAL)16384.;
	REAL f = (REAL)1000. / (REAL)2. / (REAL)M_PI;
	for( ulong i = 0; i < n; i++ )
	{
		a[i].real((REAL)1000.*cos((REAL)2. * (REAL)M_PI * (REAL)i * f / f_descr) );
		a[i].imag((REAL)1000.*sin((REAL)2. * (REAL)M_PI * (REAL)i * f / f_descr) );
//		a[i].imag( 0. );
	}
	memcpy( b, a, sizeof(b) );

	fft_integral_complex( b, n, f_descr, namespace_fft::SingFFTStatic );

	FILE *logf = fopen("log.int", "wt");
	for( ulong i = 0; i < n; i++ )
	{
		Complex val((REAL)1000.*sin((REAL)2. * (REAL)M_PI * (REAL)i * f / f_descr ),
			(REAL)-1000.*cos((REAL)2. * (REAL)M_PI * (REAL)i * f / f_descr ) );
		Complex val1 = 1000.0f * b[i]; // переход от м/с2 к мм/с
		fprintf( logf, "%f\t%f\t%f\t%f\n", val.real(), val1.real(), val.imag(), val1.imag() );
//		if( fabs ( val1.real() - val.real() ) / 1000. > INT_DIFF_PREC || 
//			fabs ( val1.imag() - val.imag() ) / 1000. > INT_DIFF_PREC )
//			return false;
	}
	fclose(logf);
	return true;
}

COREDSP_API bool TestIntegrDiff()
{
//	return TestIntegrReal();
	return TestIntegrComplex();
}
#endif


}//namespace_utilfunc