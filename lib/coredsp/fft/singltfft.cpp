/*  --------------------------------------------------------------------
*
*  Multivariate complex fourier transform, computed in place
*  using mixed-radix fast fourier transform algorithm.
*
*  Translated from Fortran to C++ by
*  A. Sazonov (sazonov@thsun1.jinr.ru)
*  March 2000.
*
*  Original Fortran code by
*  R. C. Singleton, Stanford Research Institute, Sept. 1968
*  see source at http://www.netlib.org/go/fft.f
*  or           http://www.numis.nwu.edu/ftp/pub/transforms/fft.f
*
*  -------------------------------------------------------------------
*/

#include <math.h>

#include "complstd.h"
#include "commdsp.h"
#include "singltfft.h"
#include "dspexcept.h"

namespace namespace_fft
{

#define MAX_FACTOR 209
#define FACTOR_NUMBERS 37

const long autoArraySize = ( FACTOR_NUMBERS + 1 )*sizeof( double ) * 4 
					+ ( MAX_FACTOR + 1 )*sizeof( int );

void factorize( int *nfac, int &maxp, int &maxf, int &m, int &kt, int k )
{
	while ( !( k % 16 ) )
	{ 
		m++; 
		nfac[m] = 4; 
		k /= 16; 
	}

	int j = 3, jj = 9;

	do
	{
		while( !( k % jj ) )
		{ 
			m++;
			nfac[m] = j;
			k /= jj;
		}
		j += 2;
		jj = j * j;
	}
	while ( jj <= k );

	if( k <= 4 )
	{
		kt = m;
		nfac[m+1] = k;
		if (k != 1)
			m++;
	}
	else
	{
		if ( !( k % 4 ) )
		{ 
			m++; 
			nfac[m] = 2; 
			k /= 4;
		}

		kt = m;
		j = 2;

		do
		{
			if( !( k % j ) )
			{ 
				m++; 
				nfac[m] = j; 
				k /= j; 
			}
			j = ( ( j + 1 ) / 2 ) * 2 + 1;
		}
		while ( j <= k );
	}
	if ( kt != 0 )
	{
		j = kt; 
		do 
		{ 
			m++; 
			nfac[m] = nfac[j]; 
			j--; 
		}
		while ( j != 0 ); 
	}

	//  find maximum prime factor
	for ( int itm = 1; itm < FACTOR_NUMBERS; itm++ )
		if ( nfac[itm] > maxf ) 
			maxf = nfac[itm];
}


void singleton_fft( Complex *f, int ntot, int n, int nspan, int isn, DSPMemoryAllocator *alloc, char* staticArray )
{
	/* ----------------------------------------------------------------
	*  Indexing of external arrays a[] and b[] is changed according to C++
	*  requirements, internal arrays ( nfac[], np[], at[], ck[], bt[], sk[] )
	*  are still using index starting from 1.
	*
	*  A.Sazonov (sazonov@thsun1.jinr.ru)
	* -----------------------------------------------------------------
	*/

	/*
	c  multivariate complex fourier transform, computed in place
	c    using mixed-radix fast fourier transform algorithm.
	c  by r. c. singleton, stanford research institute, sept. 1968
	c  arrays a and b originally hold the real and imaginary
	c    components of the data, and return the real and
	c    imaginary components of the resulting fourier coefficients.
	c  multivariate data is indexed according to the fortran
	c    array element successor function, without limit
	c    on the number of implied multiple subscripts.
	c    the subroutine is called once for each variate.
	c    the calls for a multivariate transform may be in any order.
	c  ntot is the total number of complex data values.
	c  n is the dimension of the current variable.
	c  nspan/n is the spacing of consecutive data values
	c    while indexing the current variable.
	c  the sign of isn determines the sign of the complex
	c    exponential, and the magnitude of isn is normally one.
	c  a tri-variate transform with a(n1,n2,n3), b(n1,n2,n3)
	c    is computed by
	c      call fft(a,b,n1*n2*n3,n1,n1,1)
	c      call fft(a,b,n1*n2*n3,n2,n1*n2,1)
	c      call fft(a,b,n1*n2*n3,n3,n1*n2*n3,1)
	c  for a single-variate transform,
	c    ntot = n = nspan = (number of complex data values), e.g.
	c      call fft(a,b,n,n,n,1)
	c  the data can alternatively be stored in a single complex array c
	c    in standard fortran fashion, i.e. alternating real and imaginary
	c    parts. then with most fortran compilers, the complex array c can
	c    be equivalenced to a real array a, the magnitude of isn changed
	c    to two to give correct indexing increment, and a(1) and a(2) used
	c    to pass the initial addresses for the sequences of real and
	c    imaginary values, e.g.
	c       complex c(ntot)
	c       real    a(2*ntot)
	c       equivalence (c(1),a(1))
	c       call fft(a(1),a(2),ntot,n,nspan,2)
	c  arrays at(maxf), ck(maxf), bt(maxf), sk(maxf), and np(maxp)
	c    are used for temporary storage.  if the available storage
	c    is insufficient, the program is terminated by a stop.
	c    maxf must be .ge. the maximum prime factor of n.
	c    maxp must be .gt. the number of prime factors of n.
	c    in addition, if the square-free portion k of n has two or
	c    more prime factors, then maxp must be .ge. k-1.
	c  ***************************************
	*/

	//---------------------------------------------------------------
	// Notes below are added by A.Sazonov, May 2000:
	//---------------------------------------------------------------
	//  arrays at[maxf], ck[maxf], bt[maxf], sk[maxf], and np[maxp]
	//  are used for temporary storage.
	//     maxf must be >=   the maximum prime factor of n.
	//     maxp must be > the number of prime factors of n.
	//  in addition, if the square-free portion k of n has two or
	//  more prime factors, then must be maxp >= k-1.
	//---------------------------------------------------------------
	// The advantage of C++ dynamic memory allocation is used
	// to create arrays with sufficient lengths.
	// The lengths of arrays at[maxf], ck[maxf], bt[maxf], sk[maxf]
	// may be calculated at the begining of program. 
	// The array np[maxp] initially created with length maxp+1
	// will be resized as soon as it will be found insufficient
	// for further calculations.
	//---------------------------------------------------------------
	int maxp = MAX_FACTOR;// initial value
	int maxf = 1;           // maxf will be adjusted later
	//  array storage in 'nfac' for a maximum of 32 prime factors of n.
	//  it's surely enough if n <= 2^32, and minimal factor is 2
	int nfac[FACTOR_NUMBERS + 1];

	if ( !alloc && !staticArray )
		THROW_EXCEPTION( "No memory for fft function" );
	if ( n < 2 )
		THROW_EXCEPTION( "Length must be greater than 2" );

	int inc = isn;
	double c72=0.30901699437494742;// cos(72 deg)
	double s72=0.95105651629515357;// sin(72 deg)
	double s120=0.86602540378443865;// sin(120 deg)
	double pi=3.141592653589793;
	double rad=2.*pi;
	double c1, c2, c3, s1, s2, s3;

	if ( isn < 0 )
	{
		s72 = -s72; 
		s120 = -s120; 
		rad = -rad; 
		inc = -inc;
	}

	int nt = inc * ntot;
	int ks = inc * nspan;
	int kspnn, kspan = ks;
	int nn = nt - inc;
	int jc = ks / n;

	double radf = rad * double(jc) * 0.5;
	int i = 0, jf = 0;

	for ( int itm = 1; itm < FACTOR_NUMBERS; itm++ )
		nfac[itm] = 0;

	//  determine the factors of n
	int kt, m = 0, k = n;
	int j, jj;
	factorize( nfac, maxp, maxf, m, kt, k );

	if ( !alloc && maxf > FACTOR_NUMBERS )
		THROW_EXCEPTION2( "Maximum factor greater than %d", MAX_FACTOR );

	//  compute Fourier transform
	int kk, k1, k2, k3, k4;
	double sd, cd;
	double aa, bb, ak, bk, aj, bj;
	double ajm, ajp, akm, akp, bjm, bjp, bkm, bkp;

	double	*at;
	double	*bt;
	double	*ck;
	double	*sk;
	int		*np;
	// allocate temporary arrays, here np[] is not quaranteed to have enough size
	// but it will be adjusted if data will not fit default size
	if ( alloc )
	{
		at = alloc->Alloc<double>( maxf + 1 );
		bt = alloc->Alloc<double>( maxf + 1 );
		ck = alloc->Alloc<double>( maxf + 1 );
		sk = alloc->Alloc<double>( maxf + 1 );
		np = alloc->Alloc<int>( maxp + 1 );
	}
	else
	{
		at = (double*)staticArray;
		bt = at + FACTOR_NUMBERS + 1;
		ck = bt + FACTOR_NUMBERS + 1;
		sk = ck + FACTOR_NUMBERS + 1;
		np = ( int * )( sk + FACTOR_NUMBERS + 1 );
	}

L100:
	sd = radf / double( kspan );
	cd = sin(sd);
	cd *= cd;
	cd *= 2.;
	sd = sin( sd + sd );
	kk = 0;
	i++;

	// transform for factor of 2  (including rotation factor)
	if ( nfac[i] != 2 ) 
		goto L400;

	kspan = kspan / 2;
	k1 = kspan + 2 - 1;
	do
	{
		do
		{
			k2 = kk + kspan;
			ak = f[k2].real();
			bk = f[k2].imag();
			f[k2].real( f[kk].real() - ak );
			f[k2].imag( f[kk].imag() - bk );
			f[kk].real( f[kk].real() + ak );
			f[kk].imag( f[kk].imag() + bk );
			kk = k2 + kspan;
		}
		while ( kk <= nn - 1 );
		kk=kk-nn;
	} 
	while ( kk <= jc - 1 );
	if ( kk > ( kspan - 1 ) )
		goto L800;

	do
	{  
		c1=1.-cd;
		s1=sd;

		do
		{
			do
			{ 
				do
				{ 
					k2=kk+kspan;
					ak=f[kk].real()-f[k2].real();        
					bk=f[kk].imag()-f[k2].imag();
					f[kk].real( f[kk].real()+f[k2].real() );
					f[kk].imag( f[kk].imag()+f[k2].imag() );
					f[k2].real( c1*ak-s1*bk );
					f[k2].imag( s1*ak+c1*bk );
					kk=k2+kspan;

				}
				while (kk < nt-1);
				k2=kk-nt;
				c1=-c1;
				kk=k1-k2-1;
			}
			while (kk > k2);
			ak=c1-(cd*c1+sd*s1);
			s1=(sd*c1-cd*s1)+s1;
			c1=2.-(ak*ak+s1*s1);
			s1=c1*s1;
			c1=c1*ak;
			kk+=jc;
		}
		while ( kk < k2 );
		k1+=inc; 
		k1+=inc;
		kk=(k1-kspan+1)/2+jc-1;
	}
	while (kk <= jc+jc-1);
	goto L100;

	//  transform for factor of 3 (optional code)
L320:
	do
	{ 
		do
		{ 
			k1=kk+kspan;            
			k2=k1+kspan;
			ak=f[kk].real();               
			bk=f[kk].imag();
			aj=f[k1].real()+f[k2].real();         
			bj=f[k1].imag()+f[k2].imag();
			f[kk].real( ak+aj );            
			f[kk].imag( bk+bj );
			ak=-0.5*aj+ak;          
			bk=-0.5*bj+bk;
			aj=(f[k1].real()-f[k2].real())*s120;  
			bj=(f[k1].imag()-f[k2].imag())*s120;
			f[k1].real( ak-bj );            
			f[k1].imag( bk+aj );
			f[k2].real( ak+bj );            
			f[k2].imag( bk-aj );
			kk=k2+kspan;
		} 
		while ( kk < nn -1 );
		kk=kk-nn;
	}   
	while ( kk <= kspan -1 );
	goto L700;

	//  transform for factor of 4
L400:
	if ( nfac[i] != 4 )
		goto L600;

	kspnn=kspan;
	kspan=kspan/4;
L410:
	c1=1.;
	s1=0.;
L420:
	k1=kk+kspan;      
	k2=k1+kspan;      
	k3=k2+kspan;
	akp=f[kk].real()+f[k2].real();  
	akm=f[kk].real()-f[k2].real();
	ajp=f[k1].real()+f[k3].real();  
	ajm=f[k1].real()-f[k3].real();
	f[kk].real(akp+ajp);
	ajp=akp-ajp;
	bkp=f[kk].imag()+f[k2].imag();  
	bkm=f[kk].imag()-f[k2].imag();
	bjp=f[k1].imag()+f[k3].imag();  
	bjm=f[k1].imag()-f[k3].imag();
	f[kk].imag(bkp+bjp);
	bjp=bkp-bjp;

	if ( isn < 0 ) 
		goto L450;

	akp=akm-bjm;      
	akm=akm+bjm;
	bkp=bkm+ajm;      
	bkm=bkm-ajm;
	if ( s1 == 0 ) 
		goto L460;
L430:
	f[k1].real( akp*c1-bkp*s1 );    
	f[k1].imag( akp*s1+bkp*c1 );
	f[k2].real( ajp*c2-bjp*s2 );    
	f[k2].imag( ajp*s2+bjp*c2 );
	f[k3].real( akm*c3-bkm*s3 );    
	f[k3].imag( akm*s3+bkm*c3 );
	kk=k3+kspan;

	if ( kk <= nt-1 ) 
		goto L420;
L440:
	c2=c1-(cd*c1+sd*s1);
	s1=(sd*c1-cd*s1)+s1;
	c1=2.0-(c2*c2+s1*s1);
	s1*=c1;
	c1*=c2;
	c2=c1*c1-s1*s1;
	s2=2.0*c1*s1;
	c3=c2*c1-s2*s1;
	s3=c2*s1+s2*c1;
	kk-=nt; kk+=jc;

	if ( kk <= kspan-1) 
		goto L420;
	kk-=kspan; 
	kk+=inc;

	if ( kk <= jc-1 ) 
		goto L410;
	if ( kspan == jc ) 
		goto L800;

	goto L100;

L450:
	akp=akm+bjm;   
	akm=akm-bjm;
	bkp=bkm-ajm;   
	bkm=bkm+ajm;

	if ( s1 != 0) 
		goto L430;

L460:
	f[k1].real( akp );     
	f[k1].imag( bkp );
	f[k2].real( ajp );     
	f[k2].imag( bjp );
	f[k3].real( akm );     
	f[k3].imag( bkm );
	kk=k3+kspan;

	if ( kk <= nt-1 ) 
		goto L420;

	goto L440;

	//  transform for factor of 5 (optional code)
L510:
	c2=c72*c72-s72*s72;
	s2=2.0*c72*s72;
	do
	{ 
		do
		{ 
			k1=kk+kspan; 
			k2=k1+kspan; 
			k3=k2+kspan;  
			k4=k3+kspan;
			akp=f[k1].real()+f[k4].real();          
			akm=f[k1].real()-f[k4].real();
			bkp=f[k1].imag()+f[k4].imag();          
			bkm=f[k1].imag()-f[k4].imag();
			ajp=f[k2].real()+f[k3].real();          
			ajm=f[k2].real()-f[k3].real();
			bjp=f[k2].imag()+f[k3].imag();          
			bjm=f[k2].imag()-f[k3].imag();
			aa=f[kk].real();                 
			bb=f[kk].imag();
			f[kk].real( aa+akp+ajp );         
			f[kk].imag( bb+bkp+bjp );
			ak=akp*c72+ajp*c2+aa;     
			bk=bkp*c72+bjp*c2+bb;
			aj=akm*s72+ajm*s2;        
			bj=bkm*s72+bjm*s2;
			f[k1].real( ak-bj );              
			f[k4].real( ak+bj );
			f[k1].imag( bk+aj );              
			f[k4].imag( bk-aj );
			ak=akp*c2+ajp*c72+aa;     
			bk=bkp*c2+bjp*c72+bb;
			aj=akm*s2-ajm*s72;        
			bj=bkm*s2-bjm*s72;
			f[k2].real( ak-bj );              
			f[k3].real( ak+bj );
			f[k2].imag( bk+aj );              
			f[k3].imag( bk-aj );
			kk=k4+kspan;
		}
		while ( kk < nn-1 );
		kk=kk-nn;
	}
	while ( kk <= kspan-1 ) ;
	goto L700;

	//  transform for odd factors
L600:
	k=nfac[i];
	kspnn=kspan;
	kspan=kspan/k;
	if ( k == 3 ) 
		goto L320;
	if ( k == 5 ) 
		goto L510;
	if ( k == jf ) 
		goto L640;
	jf=k;
	s1=rad/double(k);
	c1=cos(s1);       
	s1=sin(s1);
	if ( jf > maxf ) 
		goto L998 ;
	ck[jf]=1.0;       
	sk[jf]=0.0;
	j=1;

	do
	{
		ck[j]=ck[k]*c1+sk[k]*s1;
		sk[j]=ck[k]*s1-sk[k]*c1;
		k=k-1;
		ck[k]=ck[j];
		sk[k]=-sk[j];
		j=j+1;
	}  
	while ( j < k );
L640:
	do
	{ 
		do
		{ 
			k1=kk;
			k2=kk+kspnn;
			aa=f[kk].real();   bb=f[kk].imag();
			ak=aa;      bk=bb;
			j=1;
			k1=k1+kspan;

			do
			{ 
				k2=k2-kspan;
				j++;
				at[j]=f[k1].real()+f[k2].real();
				ak=at[j]+ak;
				bt[j]=f[k1].imag()+f[k2].imag();
				bk=bt[j]+bk;
				j++;
				at[j]=f[k1].real()-f[k2].real();
				bt[j]=f[k1].imag()-f[k2].imag();
				k1=k1+kspan;
			}  
			while ( k1 < k2 );

			f[kk].real( ak );
			f[kk].imag( bk );
			k1=kk;
			k2=kk+kspnn;
			j=1;

			do
			{ 
				k1+=kspan;   
				k2-=kspan;
				jj=j;
				ak=aa;       
				bk=bb;
				aj=0.0;      
				bj=0.0;
				k=1;

				do
				{
					k++;
					ak=at[k]*ck[jj]+ak;
					bk=bt[k]*ck[jj]+bk;
					k++;
					aj=at[k]*sk[jj]+aj;
					bj=bt[k]*sk[jj]+bj;
					jj+=j;
					if ( jj > jf ) 
						jj-=jf;

				}
				while ( k < jf );

				k=jf-j;
				f[k1].real( ak-bj );  
				f[k1].imag( bk+aj );
				f[k2].real( ak+bj );  
				f[k2].imag( bk-aj );
				j++;

			}
			while( j < k );
			kk=kk+kspnn;
		}
		while ( kk <= nn-1 );
		kk-=nn;
	}
	while ( kk <= kspan-1 );

	//  multiply by rotation factor (except for factors of 2 and 4);
L700:
	if ( i == m ) 
		goto L800;
	kk=jc;
	do
	{ 
		c2=1.0-cd;
		s1=sd;
		do
		{ 
			c1=c2;
			s2=s1;
			kk+=kspan;

			do
			{
				do
				{ 
					ak=f[kk].real();
					f[kk].real( c2*ak-s2*f[kk].imag() );
					f[kk].imag( s2*ak+c2*f[kk].imag() );
					kk+=kspnn;
				}
				while ( kk <= nt-1 );

				ak=s1*s2;
				s2=s1*c2+c1*s2;
				c2=c1*c2-ak;
				kk=kk-nt+kspan;
			}
			while ( kk <= kspnn-1 );

			c2=c1-(cd*c1+sd*s1);
			s1=s1+(sd*c1-cd*s1);
			c1=2.0-(c2*c2+s1*s1);
			s1=c1*s1;
			c2=c1*c2;
			kk=kk-kspnn+jc;
		}
		while ( kk <= kspan -1 );

		kk=kk-kspan+jc+inc;
	}
	while ( kk <= jc+jc-1 );

	goto L100;

	//  permute the results to normal order---done in two stages
	//  permutation for square factors of n

L800:
	np[1]=ks;
	if ( kt == 0) 
		goto L890;
	k=kt+kt+1;
	if ( m < k )
		k=k-1;
	j=1;
	np[k+1]=jc;

	do
	{ 
		np[j+1]=np[j]/nfac[j];
		np[k]=np[k+1]*nfac[j];
		j++;
		k--;
	} 
	while ( j < k );

	k3=np[k+1]-1;
	kspan=np[2];
	kk=jc;
	k2=kspan;
	j=1;
	if ( n != ntot )
		goto L850;
	//  permutation for single-variate transform (optional code)
L820:
	do
	{ 
		ak=f[kk].real();
		f[kk].real( f[k2].real() );
		f[k2].real( ak );
		bk=f[kk].imag();
		f[kk].imag( f[k2].imag() );
		f[k2].imag( bk );
		kk+=inc;
		k2+=kspan;
	} 
	while ( k2 < ks-1 );
L830:
	do
	{
		k2-=np[j];
		j++;
		k2+=np[j+1];
	}
	while ( k2 > np[j]-1 );
	j=1;

L840: if ( kk < k2 ) 
		  goto L820;
	  kk+=inc;
	  k2+=kspan;
	  if ( k2 < ks-1 ) 
		  goto L840;
	  if ( kk < ks-1 ) 
		  goto L830;
	  jc=k3+1;
	  goto L890;

	  //  permutation for multivariate transform;
L850:
	  do
	  { 
		  do
		  { 
			  k=kk+jc+1;
			  do
			  { 
				  ak=f[kk].real();  
				  f[kk].real( f[k2].real() );  
				  f[k2].real( ak );
				  bk=f[kk].imag();  
				  f[kk].imag( f[k2].imag() );  
				  f[k2].imag( bk );
				  kk+=inc;   
				  k2+=inc;
			  }
			  while ( kk < k-1 );

			  kk=kk+ks-jc;
			  k2=k2+ks-jc;
		  }
		  while ( kk < nt-1 );

		  k2=k2-nt+kspan;
		  kk=kk-nt+jc;
	  }
	  while ( k2 < ks-1 );
L870:
	  do 
	  { 
		  k2-=np[j]; 
		  j++;  
		  k2+=np[j+1]; 
	  }
	  while ( k2 > np[j]-1 );

	  j=1;
L880:
	  if ( kk < k2 ) 
		  goto L850;
	  kk+=jc;
	  k2+=kspan;
	  if ( k2 < ks-1 ) 
		  goto L880;
	  if ( kk < ks-1 ) 
		  goto L870;

	  jc=k3+1;
L890:
	  if ( 2*kt+1 >= m ) 
	  {
		  if ( alloc )
		  {
			  alloc->Free( np );
			  alloc->Free( at );
			  alloc->Free( bt );
			  alloc->Free( ck );
			  alloc->Free( sk );
		  }
		  return;
	  }
	  kspnn = np[kt+1];

	  //  permutation for square-free factors of n;
	  j=m-kt;
	  nfac[j+1]=1;

	  //L900:
	  do 
	  { 
		  nfac[j] = nfac[j] * nfac[j+1];
		  j--; 
	  } 
	  while ( j != kt );

	  kt++;
	  nn = nfac[kt]-1;
	  if ( nn > maxp )
	  { 
		  if ( alloc )
		  {
			  maxp=nn;
			  alloc->Free( np );
			  np = alloc->Alloc<int>( maxp + 1 );
		  }
		  else
			  goto L998;
	  }
	  jj = 0;
	  j = 0;
	  goto L906;
L902:
	  jj = jj - k2 - 1;
	  k2 = kk;
	  k++;
	  kk = nfac[k]-1;
L904:
	  jj = kk+jj+1;
	  if ( jj-1 >= k2 ) 
		  goto L902;
	  np[j]=jj;
L906:
	  k2=nfac[kt]-1;
	  k=kt+1;
	  kk=nfac[k]-1;
	  j++;
	  if ( j <= nn ) 
		  goto L904;
	  //  determine the permutation cycles of length greater than 1;
	  j=0;
	  goto L914;
L910:
	  do 
	  { 
		  k=kk+1;  
		  kk=np[k]-1;  
		  np[k]=-kk-1; 
	  }
	  while ( kk != j-1 );

	  k3=kk;
L914:
	  do 
	  { 
		  j++;  
		  kk=np[j]-1; 
	  }
	  while ( kk < -1 );

	  if ( kk != j-1 ) 
		  goto L910;
	  np[j]=-j;
	  if ( j != nn ) 
		  goto L914;
	  maxf=inc*maxf;
	  //  reorder a and b, following the permutation cycles;
	  goto L950;

L924:
	  do
	  { 
		  do 
		  { 
			  j--; 
		  }
		  while ( np[j] < 0 );

		  jj=jc;
		  do
		  { 
			  kspan=jj;
			  if ( jj > maxf ) 
				  kspan=maxf;
			  jj-=kspan;
			  k=np[j];
			  kk=jc*k+i+jj-1;
			  k1=kk+kspan;
			  k2=0;
			  do
			  { 
				  at[k2]=f[k1].real(); 
				  bt[k2]=f[k1].imag(); 
				  k2++; 
				  k1-=inc; 
			  }
			  while ( k1 != kk );

			  do
			  { 
				  k1=kk+kspan;
				  k2=k1-jc*(k+np[k]);
				  k=-np[k];
				  do
				  { 
					  f[k1].real( f[k2].real() ); 
					  f[k1].imag( f[k2].imag() ); 
					  k1-=inc; 
					  k2-=inc; 
				  }
				  while( k1 != kk );
				  kk=k2;
			  } 
			  while ( k != j );

			  k1=kk+kspan;
			  k2=0;
			  do
			  { 
				  f[k1].real( at[k2] ); 
				  f[k1].imag( bt[k2] ); 
				  k2++; 
				  k1-=inc; 
			  }
			  while ( k1 != kk );

		  } 
		  while ( jj != 0 );
	  } 
	  while ( j != 1 );
L950:
	  j=k3+2;
	  nt=nt-kspnn;
	  i=nt-inc+1;
	  if ( nt >= 0 ) 
		  goto L924;
	  if ( alloc )
	  {
		  alloc->Free( np );
		  alloc->Free( at );
		  alloc->Free( bt );
		  alloc->Free( ck );
		  alloc->Free( sk );
	  }
	  return;

	  //  error finish, insufficient array storage;
L998:
	  if ( alloc )
	  {
		  alloc->Free( np );
		  alloc->Free( at );
		  alloc->Free( bt );
		  alloc->Free( ck );
		  alloc->Free( sk );
	  }
	  isn = 0;
	  THROW_EXCEPTION( "Error: array bounds exceeded within function fft" );
}

COREDSP_API bool IsSingFFTLength( ulong n )
{
	int maxp = MAX_FACTOR;
	int maxf = 1;
	int nfac[FACTOR_NUMBERS + 1];
	if ( n < 2 )
		return false;

	for ( int itm = 1; itm < FACTOR_NUMBERS; itm++ )
		nfac[itm] = 0;

	int kt, m = 0, k = (int)n;
	factorize( nfac, maxp, maxf, m, kt, k );
//  сортировка факторов
	for ( int itm = 1; itm <= m; itm++ )
		for ( int itm1 = 1; itm1 <= m - itm; itm1++ )
			if( nfac[itm1] > nfac[itm1 + 1] )
			{
				int t = nfac[itm1 + 1];
				nfac[itm1 + 1] = nfac[itm1];
				nfac[itm1] = t;
			}

	int npermutefactor = 1;
	int lastfac = 0;
	int factorpower = 1;
	for ( int itm = 1; itm <= m; itm++ )
	{
		if( lastfac != nfac[itm] )
		{
			// новый фактор
			if( lastfac && ( factorpower % 2 ) )
				npermutefactor *= lastfac;

			lastfac = nfac[itm];
			factorpower = 1;
		}
		else
			factorpower++;
	}
	if( lastfac && ( factorpower % 2 ) )
		npermutefactor *= lastfac;


	if ( maxf > FACTOR_NUMBERS || npermutefactor > MAX_FACTOR + 1 )
		return false;
	return true;
}

COREDSP_API void SingFFTStatic( Complex *f, ulong n, int isn )
{
	char buffer[autoArraySize];
	singleton_fft( f, n, n, n, isn, NULL, buffer );
}

COREDSP_API void SingFFTDynamic( Complex *f, ulong n, int isn )
{
	singleton_fft( f, n, n, n, isn, defaultAllocator, NULL );
}

COREDSP_API void SingFFTDynamicAlloc( Complex *f, ulong n, int isn, DSPMemoryAllocator *alloc )
{
	singleton_fft( f, n, n, n, isn, alloc, NULL );
}

COREDSP_API ulong GetSingFftLength( ulong n, int isign )
{
	while ( n )
	{
		if( IsSingFFTLength( n ) )
			return n;
		n += isign;
	}
	return 0;
}

#ifdef USE_TEST_PROC
COREDSP_API bool TestSingFft()
{
	return false;
}
#endif

} //namespace_fft

