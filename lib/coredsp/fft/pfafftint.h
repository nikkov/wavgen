#ifndef __PFAFFT_TRANSFORM_INT__
#define __PFAFFT_TRANSFORM_INT__

#include "complstd.h"

/* ****************************************************************************
Return smallest valid n not less than nmin for prime factor fft.
**************************************************************************** */
ulong npfa(ulong nmin);

/* ****************************************************************************
Return optimal n between nmin and nmax for prime factor fft.
*****************************************************************************/
ulong npfao(ulong nmin, ulong nmax);

#ifdef __USE_PFA_RFFT_
/* ****************************************************************************
Return smallest valid n not less than nmin for real-to-Complex or 
Complex-to-real prime factor ffts.
******************************************************************************/
ulong npfar(ulong nmin);

/* ****************************************************************************
Return optimal n between nmin and nmax for real-to-Complex or 
Complex-to-real prime factor ffts
******************************************************************************/
ulong npfaro( ulong nmin, ulong nmax );
#endif //__USE_PFA_RFFT_

/* ****************************************************************************
Prime factor fft:  Complex to Complex transform, in place
*****************************************************************************/
void pfacc( Complex *cz, ulong n, int isign );

#ifdef __USE_PFA_RFFT_
/* ****************************************************************************
Prime factor fft:  Complex to real transform
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
n           length of transform (see notes below)
cz          array[n/2+1] of Complex values (may be equivalenced to rz)
Output:
rz          array[n] of real values (may be equivalenced to cz)
******************************************************************************/
void pfacr( Complex *cz, REAL *rz, ulong n, int isign );

/* ****************************************************************************
Prime factor fft:  real to Complex transform
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
n           length of transform; must be even (see notes below)
rz          array[n] of real values (may be equivalenced to cz)
Output:
cz          array[n/2+1] of Complex values (may be equivalenced to rz)
******************************************************************************/
void pfarc( REAL *rz, Complex *cz, ulong n, int isign );
#endif //__USE_PFA_RFFT_

#ifdef USE_NDIM_FFT_TRANSFORM
/* ****************************************************************************
Prime factor fft:  multiple Complex to Complex transforms, in place
******************************************************************************
Input:
isign       	sign of isign is the sign of exponent in fourier kernel
n           	number of Complex elements per transform (see notes below)
nt          	number of transforms
k           	stride in Complex elements within transforms
kt          	stride in Complex elements between transforms
z           	array of Complex elements to be transformed in place
Output:
z		array of Complex elements transformed
******************************************************************************/
void pfamcc( Complex *cz, ulong n, ulong nt, ulong k, ulong kt, int isign );

/* ****************************************************************************
Prime factor fft:  2-D Complex to Complex transforms, in place
******************************************************************************
Input:
isign       	sign of isign is the sign of exponent in fourier kernel
idim        	dimension to transform, either 1 or 2 (see notes)
n1          	1st (fast) dimension of array to be transformed (see notes)
n2          	2nd (slow) dimension of array to be transformed (see notes)
z           	array[n2][n1] of Complex elements to be transformed in place
Output:
z		array[n2][n1] of Complex elements transformed
******************************************************************************/
void pfa2cc( Complex *cz, ulong idim, ulong n1, ulong n2, int isign );

/* ****************************************************************************
Prime factor fft:  2-D Complex to real transforms
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
idim        dimension to transform, which must be either 1 or 2 (see notes)
n1          1st (fast) dimension of array to be transformed (see notes)
n2          2nd (slow) dimension of array to be transformed (see notes)
cz          array of Complex values (may be equivalenced to rz)
Output:
rz          array of real values (may be equivalenced to cz)
**************************************************************************** */
void pfa2cr( Complex *cz, REAL *rz, ulong idim, ulong n1, ulong n2, int isign );

/* ****************************************************************************
Prime factor fft:  2-D real to Complex transforms
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
idim        dimension to transform, which must be either 1 or 2 (see notes)
n1          1st (fast) dimension of array to be transformed (see notes)
n2          2nd (slow) dimension of array to be transformed (see notes)
rz          array of real values (may be equivalenced to cz)
Output:
cz          array of Complex values (may be equivalenced to rz)
******************************************************************************/
void pfa2rc( REAL *rz, Complex *cz, ulong idim, ulong n1, ulong n2, int isign );

#endif //USE_NDIM_FFT_TRANSFORM

#endif //__PFAFFT_TRANSFORM_INT__
