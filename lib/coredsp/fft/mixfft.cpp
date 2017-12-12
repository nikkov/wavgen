
#include <math.h>
#include "export.h"
#include "mixfft.h"
#include "dspexcept.h"

namespace namespace_fft
{
/************************************************************************
fft(ulong n, double xRe[], double xIm[], double yRe[], double yIm[])
------------------------------------------------------------------------
NOTE : This is copyrighted material, Not public domain. See below.
------------------------------------------------------------------------
Input/output:
ulong n        transformation length.
double xRe[]   real part of input sequence.
double xIm[]   imaginary part of input sequence.
double yRe[]   real part of output sequence.
double yIm[]   imaginary part of output sequence.
------------------------------------------------------------------------
Function:
The procedure performs a fast discrete Fourier transform (FFT) of
a complex sequence, x, of an arbitrary length, n. The output, y,
is also a complex sequence of length n.

y[k] = sum(x[m]*exp(-i*2*pi*k*m/n), m=0..(n-1)), k=0,...,(n-1)

The largest prime factor of n must be less than or equal to the
constant maxPrimeFactor defined below.
------------------------------------------------------------------------
Implementation notes:
The general idea is to factor the length of the DFT, n, into
factors that are efficiently handled by the routines.

A number of short DFT's are implemented with a minimum of
arithmetical operations and using (almost) straight line code
resulting in very fast execution when the factors of n belong
to this set. Especially radix-10 is optimized.

Prime factors, that are not in the set of short DFT's are handled
with direct evaluation of the DFP expression.

Please report any problems to the author. 
Suggestions and improvements are welcomed.

------------------------------------------------------------------------
The following procedures are used :
factorize		:  factor the transformation length.
transTableSetup :  setup table with sofar-, actual-, and remainRadix.
permute         :  permutation allows in-place calculations.
twiddleTransf   :  twiddle multiplications and DFT's for one stage.
initTrig        :  initialise sine/cosine table.
fft_4           :  length 4 DFT, a la Nussbaumer.
fft_5           :  length 5 DFT, a la Nussbaumer.
fft_10          :  length 10 DFT using prime factor FFT.
fft_odd         :  length n DFT, n odd.
*************************************************************************/

#define  maxPrimeFactor        37
#define  maxPrimeFactorDiv2    (maxPrimeFactor+1)/2
#define  maxFactorCount        20

static double  c3_1 = -1.50000000000000E+00;  /*  c3_1 = cos(2*pi/3)-1;          */
static double  c3_2 =  8.66025403784439E-01;  /*  c3_2 = sin(2*pi/3);            */

static double  u5   =  1.25663706143592E+00;  /*  u5   = 2*pi/5;                 */
static double  c5_1 = -1.25000000000000E+00;  /*  c5_1 = (cos(u5)+cos(2*u5))/2-1;*/
static double  c5_2 =  5.59016994374948E-01;  /*  c5_2 = (cos(u5)-cos(2*u5))/2;  */
static double  c5_3 = -9.51056516295154E-01;  /*  c5_3 = -sin(u5);               */
static double  c5_4 = -1.53884176858762E+00;  /*  c5_4 = -(sin(u5)+sin(2*u5));   */
static double  c5_5 =  3.63271264002686E-01;  /*  c5_5 = (sin(u5)-sin(2*u5));    */
static double  c8   =  7.07106781186548E-01;  /*  c8 = 1/sqrt(2);    */

struct FactorRadix
{
	ulong n;							// transformation length
	ulong nFactor;						// factors number
	ulong sofarRadix[maxFactorCount];	// the product of the radices so far.
	ulong actualRadix[maxFactorCount];	// the radix handled in this stage
	ulong remainRadix[maxFactorCount];	// the product of the remaining radices
};

/*
	factorize:  factor the transformation length.
*/
ulong factorize( ulong n, ulong *actualRadix )
{
	ulong i,j,k;
	ulong nRadix;
	ulong radices[7];
	ulong factors[maxFactorCount];

	nRadix     =  6;  
	radices[1] =  2;
	radices[2] =  3;
	radices[3] =  4;
	radices[4] =  5;
	radices[5] =  8;
	radices[6] = 10;

	if (n == 1)
	{
		j = 1;
		factors[1] = 1;
	}
	else
		j = 0;
	
	i = nRadix;
	while ( ( n > 1) && ( i > 0) )
	{
		if ( ( n % radices[i] ) == 0 )
		{
			n = n / radices[i];
			j = j + 1;
			factors[j] = radices[i];
		}
		else
			i = i - 1;
	}
	if ( factors[j] == 2 )   /*substitute factors 2*8 with 4*4 */
	{   
		i = j - 1;
		while (( i > 0 ) && ( factors[i] != 8 ))
			i--;
		if ( i > 0 )
		{
			factors[j] = 4;
			factors[i] = 4;
		}
	}
	if ( n > 1 )
	{
		for ( k = 2; k < sqrt((REAL) n) + 1; k++ )
			while ( ( n % k ) == 0 )
			{
				n = n / k;
				j = j + 1;
				factors[j] = k;
			}
			if ( n > 1 )
			{
				j = j + 1;
				factors[j] = n;
			}
	}               
	for ( i = 1; i <= j; i++ )         
	{
		actualRadix[i] = factors[j - i + 1];
	}
	return j;
}   /* factorize */

/****************************************************************************
After N is factored the parameters that control the stages are generated.
For each stage we have:
sofar   : the product of the radices so far.
actual  : the radix handled in this stage.
remain  : the product of the remaining radices.
****************************************************************************/
void transTableSetup( FactorRadix* radix )
{
	radix->nFactor = factorize( radix->n, radix->actualRadix );

	if ( radix->actualRadix[1] > maxPrimeFactor )
		THROW_EXCEPTION2( "Prime factor %d is very big", radix->actualRadix[1] );

	radix->remainRadix[0] = radix->n;
	radix->sofarRadix[1] = 1;
	radix->remainRadix[1] = radix->n / radix->actualRadix[1];
	for ( ulong i = 2; i <= radix->nFactor; i++ )
	{
		radix->sofarRadix[i] = radix->sofarRadix[i-1] * radix->actualRadix[i-1];
		radix->remainRadix[i] = radix->remainRadix[i-1] / radix->actualRadix[i];
	}
}   /* transTableSetup */

/****************************************************************************
The sequence y is the permuted input sequence x so that the following
transformations can be performed in-place, and the final result is the
normal order.
****************************************************************************/
void permute( ulong n, ulong nFactor, ulong *actualRadix, ulong *remainRadix, 
			 Complex *x, Complex *y )

{
	ulong count[maxFactorCount]; 

	for ( ulong i = 1; i <= nFactor; i++ )
		count[i] = 0;

	ulong k = 0;

	for ( ulong i = 0; i <= n - 2; i++ )
	{
		y[i] = x[k];
		
		ulong j = 1;
		k = k + remainRadix[j];
		count[j]++;
		while ( count[j] >= actualRadix[j] )
		{
			count[j] = 0;
			k = k - remainRadix[j - 1] + remainRadix[j + 1];
			j++;
			count[j]++;
		}
	}
	y[n - 1] = x[n - 1];
}   /* permute */


/****************************************************************************
Twiddle factor multiplications and transformations are performed on a
group of data. The number of multiplications with 1 are reduced by skipping
the twiddle multiplication of the first stage and of the first group of the
following stages.
***************************************************************************/

void initTrig( ulong radix, double *trigRe, double *trigIm )
{
	double w = 2.*M_PI/(double)radix;
	trigRe[0] = 1.;
	trigIm[0] = 0.;

	trigRe[1] = cos(w);
	trigIm[1] = -sin(w);

	Complex x( (REAL)trigRe[1], (REAL)trigIm[1] ); 

	for ( ulong i = 2; i < radix; i++ )
	{
		trigRe[i] = x.real() * trigRe[i - 1] - x.imag() * trigIm[i - 1];
		trigIm[i] = x.imag() * trigRe[i - 1] + x.real() * trigIm[i - 1];
	}
}   /* initTrig */

inline static void fft_4( double *aRe, double *aIm )
{
	double  t1_re,t1_im, t2_re,t2_im;
	double  m2_re,m2_im, m3_re,m3_im;

	t1_re=aRe[0] + aRe[2]; t1_im=aIm[0] + aIm[2];
	t2_re=aRe[1] + aRe[3]; t2_im=aIm[1] + aIm[3];

	m2_re=aRe[0] - aRe[2]; m2_im=aIm[0] - aIm[2];
	m3_re=aIm[1] - aIm[3]; m3_im=aRe[3] - aRe[1];

	aRe[0]=t1_re + t2_re; aIm[0]=t1_im + t2_im;
	aRe[2]=t1_re - t2_re; aIm[2]=t1_im - t2_im;
	aRe[1]=m2_re + m3_re; aIm[1]=m2_im + m3_im;
	aRe[3]=m2_re - m3_re; aIm[3]=m2_im - m3_im;
}   /* fft_4 */


inline static void fft_5( double *aRe, double *aIm )
{    
	double  t1_re,t1_im, t2_re,t2_im, t3_re,t3_im;
	double  t4_re,t4_im, t5_re,t5_im;
	double  m2_re,m2_im, m3_re,m3_im, m4_re,m4_im;
	double  m1_re,m1_im, m5_re,m5_im;
	double  s1_re,s1_im, s2_re,s2_im, s3_re,s3_im;
	double  s4_re,s4_im, s5_re,s5_im;

	t1_re=aRe[1] + aRe[4]; t1_im=aIm[1] + aIm[4];
	t2_re=aRe[2] + aRe[3]; t2_im=aIm[2] + aIm[3];
	t3_re=aRe[1] - aRe[4]; t3_im=aIm[1] - aIm[4];
	t4_re=aRe[3] - aRe[2]; t4_im=aIm[3] - aIm[2];
	t5_re=t1_re + t2_re; t5_im=t1_im + t2_im;
	aRe[0]=aRe[0] + t5_re; aIm[0]=aIm[0] + t5_im;
	m1_re=c5_1*t5_re; m1_im=c5_1*t5_im;
	m2_re=c5_2*(t1_re - t2_re); m2_im=c5_2*(t1_im - t2_im);

	m3_re=-c5_3*(t3_im + t4_im); m3_im=c5_3*(t3_re + t4_re);
	m4_re=-c5_4*t4_im; m4_im=c5_4*t4_re;
	m5_re=-c5_5*t3_im; m5_im=c5_5*t3_re;

	s3_re=m3_re - m4_re; s3_im=m3_im - m4_im;
	s5_re=m3_re + m5_re; s5_im=m3_im + m5_im;
	s1_re=aRe[0] + m1_re; s1_im=aIm[0] + m1_im;
	s2_re=s1_re + m2_re; s2_im=s1_im + m2_im;
	s4_re=s1_re - m2_re; s4_im=s1_im - m2_im;

	aRe[1]=s2_re + s3_re; aIm[1]=s2_im + s3_im;
	aRe[2]=s4_re + s5_re; aIm[2]=s4_im + s5_im;
	aRe[3]=s4_re - s5_re; aIm[3]=s4_im - s5_im;
	aRe[4]=s2_re - s3_re; aIm[4]=s2_im - s3_im;
}   /* fft_5 */

inline static void fft_8( double *zRe, double *zIm )
{
	double  aRe[4], aIm[4], bRe[4], bIm[4], gem;

	aRe[0] = zRe[0];    bRe[0] = zRe[1];
	aRe[1] = zRe[2];    bRe[1] = zRe[3];
	aRe[2] = zRe[4];    bRe[2] = zRe[5];
	aRe[3] = zRe[6];    bRe[3] = zRe[7];

	aIm[0] = zIm[0];    bIm[0] = zIm[1];
	aIm[1] = zIm[2];    bIm[1] = zIm[3];
	aIm[2] = zIm[4];    bIm[2] = zIm[5];
	aIm[3] = zIm[6];    bIm[3] = zIm[7];

	fft_4(aRe, aIm); fft_4(bRe, bIm);

	gem    = c8*(bRe[1] + bIm[1]);
	bIm[1] = c8*(bIm[1] - bRe[1]);
	bRe[1] = gem;
	gem    = bIm[2];
	bIm[2] =-bRe[2];
	bRe[2] = gem;
	gem    = c8*(bIm[3] - bRe[3]);
	bIm[3] =-c8*(bRe[3] + bIm[3]);
	bRe[3] = gem;

	zRe[0] = aRe[0] + bRe[0]; zRe[4] = aRe[0] - bRe[0];
	zRe[1] = aRe[1] + bRe[1]; zRe[5] = aRe[1] - bRe[1];
	zRe[2] = aRe[2] + bRe[2]; zRe[6] = aRe[2] - bRe[2];
	zRe[3] = aRe[3] + bRe[3]; zRe[7] = aRe[3] - bRe[3];

	zIm[0] = aIm[0] + bIm[0]; zIm[4] = aIm[0] - bIm[0];
	zIm[1] = aIm[1] + bIm[1]; zIm[5] = aIm[1] - bIm[1];
	zIm[2] = aIm[2] + bIm[2]; zIm[6] = aIm[2] - bIm[2];
	zIm[3] = aIm[3] + bIm[3]; zIm[7] = aIm[3] - bIm[3];
}   /* fft_8 */

inline static void fft_10( double *zRe, double *zIm )
{
	double  aRe[5], aIm[5], bRe[5], bIm[5];

	aRe[0] = zRe[0];    bRe[0] = zRe[5];
	aRe[1] = zRe[2];    bRe[1] = zRe[7];
	aRe[2] = zRe[4];    bRe[2] = zRe[9];
	aRe[3] = zRe[6];    bRe[3] = zRe[1];
	aRe[4] = zRe[8];    bRe[4] = zRe[3];

	aIm[0] = zIm[0];    bIm[0] = zIm[5];
	aIm[1] = zIm[2];    bIm[1] = zIm[7];
	aIm[2] = zIm[4];    bIm[2] = zIm[9];
	aIm[3] = zIm[6];    bIm[3] = zIm[1];
	aIm[4] = zIm[8];    bIm[4] = zIm[3];

	fft_5(aRe, aIm); fft_5(bRe, bIm);

	zRe[0] = aRe[0] + bRe[0]; zRe[5] = aRe[0] - bRe[0];
	zRe[6] = aRe[1] + bRe[1]; zRe[1] = aRe[1] - bRe[1];
	zRe[2] = aRe[2] + bRe[2]; zRe[7] = aRe[2] - bRe[2];
	zRe[8] = aRe[3] + bRe[3]; zRe[3] = aRe[3] - bRe[3];
	zRe[4] = aRe[4] + bRe[4]; zRe[9] = aRe[4] - bRe[4];

	zIm[0] = aIm[0] + bIm[0]; zIm[5] = aIm[0] - bIm[0];
	zIm[6] = aIm[1] + bIm[1]; zIm[1] = aIm[1] - bIm[1];
	zIm[2] = aIm[2] + bIm[2]; zIm[7] = aIm[2] - bIm[2];
	zIm[8] = aIm[3] + bIm[3]; zIm[3] = aIm[3] - bIm[3];
	zIm[4] = aIm[4] + bIm[4]; zIm[9] = aIm[4] - bIm[4];
}   /* fft_10 */

inline static void fft_odd( ulong radix,  
			 double *zRe,
			 double *zIm,
			 double *vRe, 
			 double *vIm, 
			 double *wRe, 
			 double *wIm, 
			 double *trigRe, double *trigIm )
{
	double  rere, reim, imre, imim;
	int     i,j,k,n,max;

	n = radix;
	max = (n + 1)/2;
	for (j=1; j < max; j++)
	{
		vRe[j] = zRe[j] + zRe[n-j];
		vIm[j] = zIm[j] - zIm[n-j];
		wRe[j] = zRe[j] - zRe[n-j];
		wIm[j] = zIm[j] + zIm[n-j];
	}

	for (j=1; j < max; j++)
	{
		zRe[j]=zRe[0]; 
		zIm[j]=zIm[0];
		zRe[n-j]=zRe[0]; 
		zIm[n-j]=zIm[0];
		k=j;
		for (i=1; i < max; i++)
		{
			rere = trigRe[k] * vRe[i];
			imim = trigIm[k] * vIm[i];
			reim = trigRe[k] * wIm[i];
			imre = trigIm[k] * wRe[i];

			zRe[n-j] += rere + imim;
			zIm[n-j] += reim - imre;
			zRe[j]   += rere - imim;
			zIm[j]   += reim + imre;

			k = k + j;
			if (k >= n)  k = k - n;
		}
	}
	for (j=1; j < max; j++)
	{
		zRe[0]=zRe[0] + vRe[j]; 
		zIm[0]=zIm[0] + wIm[j];
	}
}   /* fft_odd */


void twiddleTransf( ulong sofarRadix, ulong radix, ulong remainRadix,
				   Complex *y, 
				   double *zRe, double *zIm, double *vRe,
				   double *vIm, double *wRe, double *wIm,
				   double *trigRe, double *trigIm,
				   double &omega,
				   double &twRe, double &twIm, double *twiddleRe, double *twiddleIm,
				   ulong &dataOffset, ulong &groupOffset, ulong &adr,
				   ulong &dataNo, ulong &twNo, ulong &groupNo, ulong &blockNo )

{   /* twiddleTransf */ 
	double  cosw, sinw, gem;

	double t1Re, t2Re, t3Re, t4Re, t5Re;
	double t1Im, t2Im, t3Im, t4Im, t5Im;
	double m1Re, m2Re, m3Re, m4Re, m5Re;
	double m1Im, m2Im, m3Im, m4Im, m5Im;
	double s1Re, s2Re, s3Re, s4Re, s5Re;
	double s1Im, s2Im, s3Im, s4Im, s5Im;

	initTrig( radix, trigRe, trigIm );
	omega = (REAL)2.*(REAL)M_PI / (REAL)( sofarRadix*radix );
	cosw =  cos(omega);
	sinw = -sin(omega);
	twRe = 1.0;
	twIm = 0.;

	dataOffset = 0;
	groupOffset = dataOffset;
	adr = groupOffset;
	for ( dataNo = 0; dataNo < sofarRadix; dataNo++ )
	{
		if (sofarRadix>1)
		{
			twiddleRe[0] = 1.0;
			twiddleIm[0] = 0.0;
			twiddleRe[1] = twRe;
			twiddleIm[1] = twIm;

			for ( twNo = 2; twNo < radix; twNo++ )
			{
				twiddleRe[twNo] = twRe * twiddleRe[twNo-1] - twIm * twiddleIm[twNo-1];
				twiddleIm[twNo] = twIm * twiddleRe[twNo-1] + twRe * twiddleIm[twNo-1];
			}
			gem   = cosw * twRe - sinw * twIm;
			twIm = sinw * twRe + cosw * twIm;
			twRe = gem;
		}
		for ( groupNo = 0; groupNo < remainRadix; groupNo++ )
		{
			if ( ( sofarRadix > 1 ) && ( dataNo > 0 ) )
			{
				zRe[0] = y[adr].real();
				zIm[0] = y[adr].imag();
				blockNo = 1;
				do
				{
					adr = adr + sofarRadix;
					zRe[blockNo] = twiddleRe[blockNo] * y[adr].real() - twiddleIm[blockNo] * y[adr].imag();
					zIm[blockNo] = twiddleRe[blockNo] * y[adr].imag() + twiddleIm[blockNo] * y[adr].real();
					blockNo++;
				} 
				while (blockNo < radix);
			}
			else
				for ( blockNo = 0; blockNo < radix; blockNo++ )
				{
					zRe[blockNo] = y[adr].real();
					zIm[blockNo] = y[adr].imag();
					adr = adr + sofarRadix;
				}
			switch(radix)
			{
			  case  2:
				  gem=zRe[0] + zRe[1];
				  zRe[1]=zRe[0] -  zRe[1];
				  zRe[0]=gem;
				  gem=zIm[0] + zIm[1];
				  zIm[1]=zIm[0] - zIm[1];
				  zIm[0]=gem;
				  break;
			  case  3:
				  t1Re=zRe[1] + zRe[2];
				  t1Im=zIm[1] + zIm[2];
				  zRe[0]=zRe[0] + t1Re;
				  zIm[0]=zIm[0] + t1Im;
				  m1Re=c3_1*t1Re;
				  m1Im=c3_1*t1Im;
				  m2Re=c3_2*(zIm[1] - zIm[2]); 
				  m2Im=c3_2*(zRe[2] -  zRe[1]);
				  s1Re=zRe[0] + m1Re;
				  s1Im=zIm[0] + m1Im;
				  zRe[1]=s1Re + m2Re;
				  zIm[1]=s1Im + m2Im;
				  zRe[2]=s1Re - m2Re;
				  zIm[2]=s1Im - m2Im;
				  break;
			  case  4:
				  t1Re=zRe[0] + zRe[2];
				  t1Im=zIm[0] + zIm[2];
				  t2Re=zRe[1] + zRe[3];
				  t2Im=zIm[1] + zIm[3];

				  m2Re=zRe[0] - zRe[2];
				  m2Im=zIm[0] - zIm[2];
				  m3Re=zIm[1] - zIm[3];
				  m3Im=zRe[3] - zRe[1];

				  zRe[0]=t1Re + t2Re;
				  zIm[0]=t1Im + t2Im;
				  zRe[2]=t1Re - t2Re;
				  zIm[2]=t1Im - t2Im;
				  zRe[1]=m2Re + m3Re;
				  zIm[1]=m2Im + m3Im;
				  zRe[3]=m2Re - m3Re;
				  zIm[3]=m2Im - m3Im;
				  break;
			  case  5:
				  t1Re=zRe[1] + zRe[4];
				  t1Im=zIm[1] + zIm[4];
				  t2Re=zRe[2] + zRe[3];
				  t2Im=zIm[2] + zIm[3];
				  t3Re=zRe[1] - zRe[4];
				  t3Im=zIm[1] - zIm[4];
				  t4Re=zRe[3] - zRe[2];
				  t4Im=zIm[3] - zIm[2];
				  t5Re=t1Re + t2Re;
				  t5Im=t1Im + t2Im;
				  zRe[0]=zRe[0] + t5Re;
				  zIm[0]=zIm[0] + t5Im;
				  m1Re=c5_1*t5Re;
				  m1Im=c5_1*t5Im;
				  m2Re=c5_2*(t1Re - t2Re); 
				  m2Im=c5_2*(t1Im - t2Im);

				  m3Re=-c5_3*(t3Im + t4Im); 
				  m3Im=c5_3*(t3Re + t4Re);
				  m4Re=-c5_4*t4Im;
				  m4Im=c5_4*t4Re;
				  m5Re=-c5_5*t3Im;
				  m5Im=c5_5*t3Re;

				  s3Re=m3Re - m4Re;
				  s3Im=m3Im - m4Im;
				  s5Re=m3Re + m5Re;
				  s5Im=m3Im + m5Im;
				  s1Re=zRe[0] + m1Re;
				  s1Im=zIm[0] + m1Im;
				  s2Re=s1Re + m2Re;
				  s2Im=s1Im + m2Im;
				  s4Re=s1Re - m2Re;
				  s4Im=s1Im - m2Im;

				  zRe[1]=s2Re + s3Re;
				  zIm[1]=s2Im + s3Im;
				  zRe[2]=s4Re + s5Re;
				  zIm[2]=s4Im + s5Im;
				  zRe[3]=s4Re - s5Re;
				  zIm[3]=s4Im - s5Im;
				  zRe[4]=s2Re - s3Re;
				  zIm[4]=s2Im - s3Im;
				  break;
				case  8  : fft_8( zRe, zIm );
					break;
				case 10  : fft_10( zRe, zIm );
					break;
				default  : fft_odd( radix, zRe, zIm, vRe, vIm, wRe, wIm, trigRe, trigIm );
					break;
			}
			adr = groupOffset;
			for ( blockNo = 0; blockNo < radix; blockNo++ )
			{
				y[adr].real( (REAL)zRe[blockNo] );
				y[adr].imag( (REAL)zIm[blockNo] );
				adr = adr + sofarRadix;
			}
			groupOffset = groupOffset + sofarRadix * radix;
			adr=groupOffset;
		}
		dataOffset = dataOffset + 1;
		groupOffset = dataOffset;
		adr = groupOffset;
	}
}   /* twiddleTransf */


COREDSP_API bool IsMixRadixFftLength( ulong n )
{
	FactorRadix param;
	param.n = n;
	param.nFactor = factorize( param.n, param.actualRadix );

	return param.actualRadix[1] <= maxPrimeFactor;
}

COREDSP_API void MixRadixFft( Complex *x, Complex *y, ulong n, int isign )
{
	FactorRadix param;
	param.n = n;

	ulong count;
	ulong groupOffset, dataOffset, adr;
	ulong groupNo, dataNo, blockNo, twNo;
	double omega;
	double twRe, twIm;

	double trigRe[maxPrimeFactor];
	double trigIm[maxPrimeFactor];

	double twiddleRe[maxPrimeFactor];
	double twiddleIm[maxPrimeFactor];

	double zRe[maxPrimeFactor];
	double zIm[maxPrimeFactor];
	double vRe[maxPrimeFactorDiv2];
	double vIm[maxPrimeFactorDiv2];
	double wRe[maxPrimeFactorDiv2];
	double wIm[maxPrimeFactorDiv2];
	
	if( isign > 0 )	//inverse transform
		for ( ulong jj = 0; jj < n; jj++ )
			x[jj].imag( -x[jj].imag() );

	transTableSetup( &param );

	permute( n, param.nFactor, param.actualRadix, param.remainRadix, x, y );
	for ( count = 1; count <= param.nFactor; count++ )
		twiddleTransf( param.sofarRadix[count], param.actualRadix[count], param.remainRadix[count],
					y, zRe, zIm, vRe, vIm, wRe, wIm, trigRe, trigIm, omega, twRe, twIm, twiddleRe, twiddleIm,
					dataOffset, groupOffset, adr, dataNo, twNo, groupNo, blockNo );

	if( isign > 0 )	//inverse transform
		for ( ulong jj = 0; jj < n; jj++ )
			y[jj].imag( -y[jj].imag() );

}   /* fft */

COREDSP_API void MixRadixFft( Complex *x, ulong n, int isign, DSPMemoryAllocator *alloc )
{
	if( !alloc )
		THROW_EXCEPTION( "Can't allocate memory without allocator" );

	Complex *y = alloc->Alloc<Complex>( n );
	try
	{
		MixRadixFft( x, y, n, isign );
	}
	catch ( DspException exept )
	{
		alloc->Free( y );
		throw exept;
	}
	
	while ( n-- ) 
		x[n] = y[n];

	alloc->Free( y );
}

COREDSP_API void MixRadixFft( Complex *x, ulong n, int isign )
{
	MixRadixFft( x, n, isign, defaultAllocator );
}

COREDSP_API ulong GetMixRadixFftLength( ulong n, int isign )
{
	while ( n )
	{
		if( IsMixRadixFftLength( n ) )
			return n;
		n += isign;
	}
	return 0;
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestMixFft()
{
	return false;
}
#endif

} //namespace namespace_fft