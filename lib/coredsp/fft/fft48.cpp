#include <math.h>
#include "dspexcept.h"
#include "complstd.h"
#include "arith.h"

namespace namespace_fft
{
static const ulong LX = 2;

inline void fft8_dif_core( Complex *f )
// 8-point decimation in frequency FFT
// isign = +1
// output data is in revbin_permuted order
//.
// cf. nussbaumer p.148f
{
	Complex t1 = f[0] + f[4];
	Complex m3 = f[0] - f[4];

	Complex t4 = f[1] + f[5];
	Complex t3 = f[1] - f[5];

	Complex t5 = f[3] + f[7];
	Complex t6 = f[3] - f[7];

	Complex t2 = f[2] + f[6];
	Complex t7 = t1 + t2;
	Complex m2 = t1 - t2;
	Complex t8 = t4 + t5;

	f[0] = t7 + t8;
	f[1] = t7 - t8;
	Complex m4 = (REAL)M_SQRT1_2 * (t3 - t6);

	// isign = +1:
	Complex m7 = Complex( 0, -M_SQRT1_2 ) * ( t3 + t6 );
	Complex m5 = ( t5 - t4 ) * Complex( 0, 1 );
	Complex m6 = ( f[6] - f[2] ) * Complex( 0, 1 );

#define s1 t1
#define s2 t2
#define s3 t3
#define s4 t4
	s1 = m3 + m4;
	s2 = m3 - m4;

	s3 = m6 + m7;
	s4 = m6 - m7;

	f[7] = s1 + s3;
	f[4] = s1 - s3;

	f[6] = s2 + s4;
	f[5] = s2 - s4;

	f[3] = m2 + m5;
	f[2] = m2 - m5;
#undef s1
#undef s2
#undef s3
#undef s4
} 

inline void fft8_dit_core( Complex *f )
// 8-point decimation in time FFT
// isign = -1
// input data must be in revbin_permuted order
//.
// cf. nussbaumer p.148f
{
	Complex t1 = f[0] + f[1];
	Complex m3 = f[0] - f[1];

	Complex t4 = f[4] + f[5];
	Complex t3 = f[4] - f[5];

	Complex t5 = f[6] + f[7];
	Complex t6 = f[6] - f[7];

	Complex t2 = f[2] + f[3];
	Complex t7 = t1 + t2;
	Complex m2 = t1 - t2;
	Complex t8 = t4 + t5;

	f[0] = t7 + t8;
	f[4] = t7 - t8;
	Complex m4 = (REAL)M_SQRT1_2 * (t3 - t6);
	Complex m7 = Complex( 0, M_SQRT1_2 ) * ( t3 + t6 );
	Complex m5 = ( t4 - t5 ) * Complex(0, 1);
	Complex m6 = ( f[2] - f[3] ) * Complex( 0, 1 );
#define s1 t1
#define s2 t2
#define s3 t3
#define s4 t4
	s1 = m3 + m4;
	s2 = m3 - m4;

	s3 = m6 + m7;
	s4 = m6 - m7;

	f[7] = s1 + s3;
	f[1] = s1 - s3;

	f[3] = s2 + s4;
	f[5] = s2 - s4;

	f[6] = m2 + m5;
	f[2] = m2 - m5;
#undef s1
#undef s2
#undef s3
#undef s4
} 


inline void fft_dif4_core( Complex *f, ulong ldn )
// auxiliary routine for fft_dif4()
// radix-4 decimation in frequency FFT
// ldn := base-2 logarithm of the array length
// isign = +1
// output data is in revbin_permuted order
{
	const ulong n = ( 1UL << ldn );

	if ( n <= 2 )
	{
		if ( n == 2 )  
			sumdiff( f[0], f[1] );
		return;
	}

	for ( ulong ldm = ldn; ldm >= ( LX << 1 ); ldm -= LX )
	{
		ulong m = ( 1UL << ldm );
		ulong m4 = ( m >> LX );

		const double ph0 = 2. * M_PI / m;

		for ( ulong j = 0; j < m4; j++ )
		{
			double phi = j * ph0;
			Complex e  = SinCos( phi );
			Complex e2 = e * e;
			Complex e3 = e2 * e;

			for ( ulong r = 0, i0 = j + r;  r < n;  r += m, i0 += m )
			{
				Complex x, y, u, v;
				ulong i1 = i0 + m4;
				ulong i2 = i1 + m4;
				ulong i3 = i2 + m4;

				sumdiff( f[i0], f[i2], x, u );
				sumdiff( f[i1], f[i3], y, v );
				v *= Complex( 0, 1 );

				diffsum3( x, y, f[i0] );
				f[i1] = y * e2;

				sumdiff( u, v, x, y );
				f[i3] = y * e3;
				f[i2] = x * e;
			}
		}
	}

	if ( ldn & 1 )  // n is not a power of 4, need a radix-8 step
	{
		for ( ulong i0 = 0; i0 < n; i0 += 8 )  
			fft8_dif_core( f + i0 );
	}
	else
	{
		for ( ulong i0 = 0; i0 < n; i0 += 4 )
		{
			ulong i1 = i0 + 1;
			ulong i2 = i1 + 1;
			ulong i3 = i2 + 1;

			Complex x, y, u, v;
			sumdiff( f[i0], f[i2], x, u );
			sumdiff( f[i1], f[i3], y, v );
			v *= Complex(0,1);
			sumdiff( x, y, f[i0], f[i1] );
			sumdiff( u, v, f[i2], f[i3] );
		}
	}
} 


inline void fft_dit4_core( Complex *f, ulong ldn )
// auxiliary routine for fft_dit4()
// radix-4 decimation in frequency fft
// ldn := base-2 logarithm of the array length
// isign = -1
// input data must be in revbin_permuted order
{
	const ulong n = ( 1UL << ldn );

	if ( n <= 2 )
	{
		if ( n == 2 )  
			sumdiff( f[0], f[1] );
		return;
	}

	ulong ldm = ldn & 1;
	if ( ldm != 0 )  // n is not a power of 4, need a radix-8 step
	{
		for ( ulong i0 = 0; i0 < n; i0 += 8 )
			fft8_dit_core( f + i0 );
	}
	else
	{
		for ( ulong i0 = 0; i0 < n; i0 += 4 )
		{
			ulong i1 = i0 + 1;
			ulong i2 = i1 + 1;
			ulong i3 = i2 + 1;

			Complex x, y, u, v;
			sumdiff( f[i0], f[i1], x, u );
			sumdiff( f[i2], f[i3], y, v );
			v *= Complex( 0, -1 );
			sumdiff( u, v, f[i1], f[i3] );
			sumdiff( x, y, f[i0], f[i2] );
		}
	}
	ldm += 2 * LX;


	for ( ; ldm <= ldn; ldm += LX )
	{
		ulong m = ( 1UL << ldm) ;
		ulong m4 = ( m >> LX );
		const double ph0 = -2.0 * M_PI / m;  // isign

		for (ulong j = 0; j < m4; j++ )
		{
			double phi = j * ph0;
			Complex e  = SinCos( phi );
			Complex e2 = e * e;
			Complex e3 = e2 * e;

			for ( ulong r = 0, i0 = j + r;  r < n;  r += m, i0 += m )
			{
				ulong i1 = i0 + m4;
				ulong i2 = i1 + m4;
				ulong i3 = i2 + m4;

				Complex x = f[i1] * e2;
				Complex u;
				sumdiff3_r( x, f[i0], u );

				Complex v = f[i3] * e3;
				Complex y = f[i2] * e;
				sumdiff( y, v );
				v *= Complex(0, -1);  // isign

				sumdiff( u, v, f[i1], f[i3] );
				sumdiff( x, y, f[i0], f[i2] );
			}
		}
	}
} 

void fft_dit4( Complex *f, ulong ldn, int is )
// Fast Fourier Transform
// ldn := base-2 logarithm of the array length
// is := sign of the transform (+1 or -1)
// radix-4 decimation in time algorithm
{
	const ulong n = 1UL << ldn;
	revbin_permute( f, n );

	fft_dit4_core( f, ldn );

	if ( is > 0 )  // note: ugly: explicit reordering
	{
		reverse_0( f, n );
	}
}

void fft_dif4( Complex *f, ulong ldn, int is )
// Fast Fourier Transform
// ldn := base-2 logarithm of the array length
// is := sign of the transform (+1 or -1)
// radix-4 decimation in frequency algorithm
{
	fft_dif4_core( f, ldn );

	const ulong n = 1UL << ldn;
	revbin_permute( f, n );

	if ( is < 0 )
		reverse_0( f, n );
} 

COREDSP_API bool IsFftDitf4Length( ulong n )
{
	return (( n & -(long)n) == n );
}

COREDSP_API void FftDit4( Complex *f, ulong n, int is )
{
	if ( !IsFftDitf4Length( n ) )
		THROW_EXCEPTION2( "Length %u isn't valid", n );
	fft_dit4( f, ld( n ), is );
}

COREDSP_API void FftDif4( Complex *f, ulong n, int is )
{
	if ( !IsFftDitf4Length( n ) )
		THROW_EXCEPTION2( "Length %u isn't valid", n );
	fft_dif4( f, ld( n ), is );
}

}//namespace namespace_fft
