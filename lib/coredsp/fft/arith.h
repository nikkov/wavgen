#ifndef __COMMON_ARITH__ 
#define __COMMON_ARITH__

#include "bitarray.h"

#define COS_1_PI_8  0.923879532511286756128183189397
#define SIN_1_PI_8  0.382683432365089771728459984029

template <typename Type>
static inline void sumdiff(Type &a, Type &b)
{ Type t=a-b; a+=b; b=t; }

template <typename Type>
static inline void sumdiff(Type a, Type b, Type &s, Type &d)
{ s=a+b; d=a-b; }

template <typename Type>
static inline void sumdiff3(Type &a, Type b, Type &d)
{ d=a-b; a+=b; } 

template <typename Type>
static inline void sumdiff3_r(Type &a, Type b, Type &d)
// NEVER call like func(a,b,a) or func(a,b,b)
{ d=b-a; a+=b; }

template <typename Type>
static inline void sumdiff05(Type &a, Type &b)
{ Type t=(a-b)*(Type)0.5; a+=b; a*=(Type)0.5; b=t; } 

template <typename Type>
static inline void sumdiff05(Type a, Type b, Type &s, Type &d)
{ s=(a+b)*(Type)0.5; d=(a-b)*(Type)0.5; } 

template <typename Type>
static inline void diffsum3(Type a, Type &b, Type &s)
// NEVER call like func(a,b,a) or func(a,b,b)
{ s=a+b; b=a-b; } 

#if defined SINCOS_USE_ASM
static inline void SinCos( double a, double *s, double *c )
{ 
#ifdef _MSC_VER 
	__asm 
	{
		fldl a
		fsincos
		movl 16 (%esp), %eax
		fstpl (%eax)
		movl 12 (%esp), %eax
		fstpl (%eax)
		ret
	}
#else
  asm(
	  "fldl 4 (%esp)"
	  "fsincos"
	  "movl 16 (%esp), %eax"
	  "fstpl (%eax)"
	  "movl 12 (%esp), %eax"
	  "fstpl (%eax)"
	  "ret"
  }
#endif
}
#else
static inline void SinCos(REAL a, REAL *s, REAL *c)
{
	*s = sin(a);
	*c = cos(a);
}
#endif //  defined SINCOS_USE_ASM 

static inline Complex SinCos(REAL phi)
{
	REAL c, s;
	SinCos(phi, &s, &c);
	return  Complex(c, s);
}

template <typename Type>
static inline void multiply(Type *f, ulong n, const Type *g)
{
	while ( n-- )  f[n] *= g[n];
} 

template <typename Type>
static inline void add(Type *f, ulong n, const Type *g)
{
	while ( n-- )  f[n] += g[n];
} 

static inline void SinCos(REAL phi, Complex *z)
{
	REAL *c = (REAL *)z;
	REAL *s = c + 1;
	SinCos(phi, s, c);
} 

static inline void csqr(REAL a, REAL b, REAL &u, REAL &v)
{ u = a*a-b*b;  v = a*b;  v += v; }

static inline void cmult_inv(REAL c, REAL s, REAL x, REAL y, REAL &u, REAL &v)
{ u = x*c+y*s;  v = y*c-x*s; }

static inline void cmult_inv(REAL c, REAL s, Complex x, Complex y, Complex &u, Complex &v)
{ u = x*c+y*s;  v = y*c-x*s; }

static inline void cmult_inv(Complex c, Complex s, Complex x, Complex y, Complex &u, Complex &v)
{ u = x*c+y*s;  v = y*c-x*s; }

static inline void cmult(REAL c, REAL s, REAL x, REAL y, REAL &u, REAL &v)
{ u = x*c-y*s;  v = y*c+x*s; }

static inline void cmult(REAL c, REAL s, Complex x, Complex y, Complex &u, Complex &v)
{ u = x*c-y*s;  v = y*c+x*s; }

static inline void cmult(Complex c, Complex s, Complex x, Complex y, Complex &u, Complex &v)
{ u = x*c-y*s;  v = y*c+x*s; } 

template <typename Type> 
static inline void  swap2(Type &x, Type &y)
{ Type t(x);  x = y;  y = t; } 

template <typename Type> 
static inline void  swap0(Type &x, Type &y)
{ y = x;  x = 0; }

#define  RBP_SYMM  4  // 1, 2, 4 (default is 4)
#define  idx_swap(f, k, r)  { ulong kx=(k), rx=(r);  swap2(f[kx], f[rx]); }

template <typename Type>
static void revbin_permute(Type *f, ulong n)
{
	if ( n<=8 )
	{
		if ( n==8 )
		{
			swap2(f[1], f[4]);
			swap2(f[3], f[6]);
		}
		else if ( n==4 )  swap2(f[1], f[2]);

		return;
	}

	const ulong nh = (n>>1);
	ulong x[BITS_PER_LONG];
	x[0] = nh;
	{  // initialize xor-table:
		ulong i, m = nh;
		for (i=1; m!=0; ++i)
		{
			m >>= 1;
			x[i] = x[i-1] ^ m;
		}
	}

#if  ( RBP_SYMM >= 2 )
	const ulong n1  = n - 1;    // = 11111111
#if  ( RBP_SYMM >= 4 )
	const ulong nx1 = nh - 2;   // = 01111110
	const ulong nx2 = n1 - nx1; // = 10111101
#endif //  ( RBP_SYMM >= 4 )
#endif //  ( RBP_SYMM >= 2 )
	ulong k=0, r=0;
	while ( k<n/RBP_SYMM  )  // n>=16, n/2>=8, n/4>=4
	{
		// ----- k%4 == 0:
		if ( r>k )
		{
			swap2(f[k], f[r]);  // <nh, <nh 11
#if  ( RBP_SYMM >= 2 )
			idx_swap(f, n1^k, n1^r);  // >nh, >nh 00
#if  ( RBP_SYMM >= 4 )
			idx_swap(f, nx1^k, nx1^r);  // <nh, <nh 11
			idx_swap(f, nx2^k, nx2^r);  // >nh, >nh 00
#endif //  ( RBP_SYMM >= 4 )
#endif //  ( RBP_SYMM >= 2 )
		}

		r ^= nh;
		++k;

		// ----- k%4 == 1:
		if ( r>k )
		{
			swap2(f[k], f[r]);  // <nh, >nh 10
#if  ( RBP_SYMM >= 4 )
			idx_swap(f, n1^k, n1^r);  // >nh, <nh 01
#endif //  ( RBP_SYMM >= 4 )
		}

		{ // scan for lowest unset bit of k:
			ulong m = 2,  i = 1;
			while ( m & k )  { m <<= 1;  ++i; }
			r ^= x[i];
		}
		++k;

		// ----- k%4 == 2:
		if ( r>k )
		{
			swap2(f[k], f[r]);  // <nh, <nh 11
#if  ( RBP_SYMM >= 2 )
			idx_swap(f, n1^k, n1^r); // >nh, >nh 00
#endif //  ( RBP_SYMM >= 2 )
		}

		r ^= nh;
		++k;

		// ----- k%4 == 3:
		if ( r>k )
		{
			swap2(f[k], f[r]);    // <nh, >nh 10
#if  ( RBP_SYMM >= 4 )
			idx_swap(f, nx1^k, nx1^r);   // <nh, >nh 10
#endif //  ( RBP_SYMM >= 4 )
		}

		{ // scan for lowest unset bit of k:
			ulong m = 4,  i = 2;
			while ( m & k )  { m <<= 1;  ++i; }
			r ^= x[i];
		}
		++k;
	}
}

#define  RBP0_SYMM  2  // 2, 4 (default is 2)

template <typename Type>
static void revbin_permute0(Type *f, ulong n)
{
	if ( n<=8 )
	{
		if ( n==8 )
		{
			swap0(f[1], f[4]);
			swap0(f[3], f[6]);
		}
		else if ( n==4 )  swap0(f[1], f[2]);

		return;
	}

	const ulong nh = (n>>1);
	ulong x[BITS_PER_LONG];
	x[0] = nh;
	{  // initialize xor-table:
		ulong i, m = nh;
		for (i=1; m!=0; ++i)
		{
			m >>= 1;
			x[i] = x[i-1] ^ m;
		}
	}

#if  ( RBP0_SYMM >= 2 )
#if  ( RBP0_SYMM >= 4 )
	const ulong n1  = n - 1;    // = 11111111
	const ulong nx1 = nh - 2;   // = 01111110
	//    const ulong nx2 = n1 - nx1; // = 10111101
#endif //  ( RBP0_SYMM >= 4 )
#endif //  ( RBP0_SYMM >= 2 )
	ulong k=0, r=0;
	while ( k<n/RBP0_SYMM  )  // n>=16, n/2>=8, n/4>=4
	{
		// ----- k%4 == 0:
		if ( r>k )
		{
			swap2(f[k], f[r]);  // <nh, <nh 11
#if  ( RBP0_SYMM >= 2 )
			//            idx_swap(f, n1^k, n1^r);  // >nh, >nh 00
#if  ( RBP0_SYMM >= 4 )
			idx_swap(f, nx1^k, nx1^r);  // <nh, <nh 11
			//            idx_swap(f, nx2^k, nx2^r);  // >nh, >nh 00
#endif //  ( RBP0_SYMM >= 4 )
#endif //  ( RBP0_SYMM >= 2 )
		}

		r ^= nh;
		++k;

		// ----- k%4 == 1:
		if ( r>k )
		{
			swap0(f[k], f[r]);  // <nh, >nh 10
#if  ( RBP0_SYMM >= 4 )
			swap0(f[r^n1], f[k^n1]);
			//            idx_swap(f, n1^k, n1^r);  // >nh, <nh 01
#endif //  ( RBP0_SYMM >= 4 )
		}

		{ // scan for lowest unset bit of k:
			ulong m = 2,  i = 1;
			while ( m & k )  { m <<= 1;  ++i; }
			r ^= x[i];
		}
		++k;

		// ----- k%4 == 2:
		if ( r>k )
		{
			swap2(f[k], f[r]);  // <nh, <nh 11
#if  ( RBP0_SYMM >= 2 )
			//            idx_swap(f, n1^k, n1^r); // >nh, >nh 00
#endif //  ( RBP0_SYMM >= 2 )
		}

		r ^= nh;
		++k;

		// ----- k%4 == 3:
		if ( r>k )
		{
			swap0(f[k], f[r]);    // <nh, >nh 10
#if  ( RBP0_SYMM >= 4 )
			swap0(f[k^nx1], f[r^nx1]);    // <nh, >nh 10
			//            idx_swap(f, nx1^k, nx1^r);   // <nh, >nh 10
#endif //  ( RBP0_SYMM >= 4 )
		}

		{ // scan for lowest unset bit of k:
			ulong m = 4,  i = 2;
			while ( m & k )  { m <<= 1;  ++i; }
			r ^= x[i];
		}
		++k;
	}
}
// -------------------------
#undef  idx_swap 



template <typename Type>
static void radix_permute( Type *f, ulong n, ulong r )
//
// swap elements with index pairs i, j were the
// radix-r representation of i and j are mutually
// digit-reversed (e.g. 436 <--> 634)
//
// This is a radix-r generalization of revbin_permute()
// revbin_permute(f, n)) =^= radix_permute(f, n, 2)
//
// must have:
//  n == p**x for some x>=1
//  r >= 2
//
// original by Andre Piotrowski
// this optimized version avoids the (n*log(n)) divisions
// by using two size-BITS_PER_LONG tables radix_permute_nt[], radix_permute_kt[]
//
{
	ulong radix_permute_nt[BITS_PER_LONG];
	ulong radix_permute_kt[BITS_PER_LONG];

	ulong x = 0;
	radix_permute_nt[0] = r-1;
	radix_permute_kt[0] = 1;
	while ( 1 )
	{
		ulong z = radix_permute_kt[x] * r;
		if ( z > n )  break;
		++x;
		radix_permute_kt[x] = z;

		radix_permute_nt[x] = radix_permute_nt[x-1] * r;
	}
	// here: n == p**x

	for (ulong i = 0, j = 0;  i < n - 1;  i++ )
	{
		if ( i < j )
			swap2( f[i], f[j] );

		ulong t = x - 1;
		ulong k = radix_permute_kt[t];  // =^=  k = (r-1) * n / r;

		while ( k <= j )
		{
			j -= k;
			k = radix_permute_nt[--t];  // =^=  k /= r;
		}

		j += radix_permute_kt[t]; // =^=  j += (k/(r-1));
	}
}


template <typename Type>
static inline void reverse_0( Type *f, ulong n )
// reverse array around index zero:
{
	if ( n <= 2 )
		return;
	for ( ulong k = 1, i = n - 1;  k < i;  ++k, --i )
	{
		Type t(f[k]);  
		f[k]=f[i];  
		f[i]=t;
	}
} 


static inline void ldn2rc(ulong ldn, ulong &nr, ulong &nc)
// input ldn == log_2(n)
// nr, nc are set that nr*nc = n and ldr>=ldc
{
	ulong ldc = (ldn>>1);
	ulong ldr = ldn-ldc;

	nc = (1UL<<ldc);
	nr = (1UL<<ldr);  // nrow >= ncol
} 

static inline ulong highest_bit_idx(ulong x)
// Return index of highest bit set.
// Return 0 if no bit is set.
{
	if ( 0==x )  return  0;
	ulong r = 0;
	if ( x & 0xffff0000 )  { x >>= 16;  r += 16; }
	if ( x & 0x0000ff00 )  { x >>=  8;  r +=  8; }
	if ( x & 0x000000f0 )  { x >>=  4;  r +=  4; }
	if ( x & 0x0000000c )  { x >>=  2;  r +=  2; }
	if ( x & 0x00000002 )  {            r +=  1; }
	return r;
} 

static inline ulong ld(ulong x)
// Return k so that 2^k <= x < 2^(k+1)
// If x==0 then 0 is returned (!)
{
	return  highest_bit_idx(x);
} 

template <typename Type1, typename Type2>
static inline void copy(const Type1 *src, Type2 *dst, ulong n)
// Copy array src[] to dst[]
{
	while ( n-- )  dst[n] = (Type2)src[n];
}

template <typename Type1, typename Type2>
static inline void copy0(const Type1 *src, ulong ns, Type2 *dst, ulong nd)
// Copy as much as makes sense, fill rest with zeros
// from src[] (length ns) to dst[] (length nd):
{
	ulong k = 0;
	ulong n = ( nd<ns ? nd : ns ); // == min(nd, ns);
	for (k=0; k<n; ++k)  dst[k] = (Type2)src[k];
	for (  ; k<nd; ++k)  dst[k] = (Type2)0;
}

template <typename Type1, typename Type2>
static inline void skip_copy(const Type1 *src, Type2 *dst, ulong n, ulong d)
// copy n elements from src[] at positions
// [0],[d],[2d],[3d],...,[(n-1)*d]
// to dst[0, 1, ... ,n-1]
{
	for (ulong k=0,j=0; j<n; k+=d,j++)  dst[j] = src[k];
}

template <typename Type1, typename Type2>
static inline void skip_copy_back(const Type1 *src, Type2 *dst, ulong n, ulong d)
// copy n elements from src[0, 1, ... ,n-1]
// to dst[] at positions
// [0],[d],[2d],[3d],...,[(n-1)*d]
{
	for (ulong k=0,j=0; j<n; k+=d,j++)  dst[k] = src[j];
} 

template <typename Type>
static inline void null(Type *f, ulong n)
// Set array to zero
{
	const Type z(0);
	while ( n-- )  f[n] = z;
}


template <typename Type>
static inline void fill(Type *dst, ulong n, Type v)
// Fill array with value v
{
	while ( n-- )  dst[n] = v;
} 

template <typename Type>
static void transpose_square(Type **f, ulong nrc)
// in-place transposition of an nr x nc array (nr = nc = nrc)
// that lies at nr rows of length nc
{
	const ulong nr = nrc,  nc = nrc;
	for (ulong r=0; r<nr; ++r)
	{
		for (ulong c=r+1; c<nc; ++c)
		{
			// swap2(f[r][c], f[c][r]):
			Type  t = f[r][c];
			f[r][c] = f[c][r];
			f[c][r] = t;
		}
	}
}

template <typename Type>
static void transpose_square(Type *f, ulong nrc)
// in-place transposition of an nr x nc array (nr = nc = nrc)
// that lies in contiguous memory
{
	const ulong nr = nrc,  nc = nrc;
	for (ulong r=0; r<nr; r++)
	{
		for (ulong c=r+1; c<nc; c++)
		{
			// swap2(f[r][c], f[c][r]):
			ulong k1 = nc * r + c;
			ulong k2 = nc * c + r;
			Type  t = f[k1];
			f[k1] = f[k2];
			f[k2] = t;
		}
	}
}

static inline ulong bit_rotate_left(ulong x, ulong r, ulong ldn)
// return ldn-bit word rotated r bits
// to the left (i.e. toward the most significant bit)
// must have  0 <= r <= ldn
{
	ulong m = ~0UL >> ( BITS_PER_LONG - ldn );
	x &= m;
	x = (x<<r) | (x>>(ldn-r));
	x &= m;
	return  x;
} 

#define  SRC(k)  bit_rotate_left(k, ldnc, ldn)

template <typename Type>
static void transpose2_ba(Type *f, ulong nr, ulong nc, bitarray *ba=0)
// in-place transposition of an  nr X nc  array
// that lies in contiguous memory
//
// use bitarray for tagging moved elements
//
{
	if ( 1>=nr )  return;
	if ( 1>=nc )  return;

	if ( nr==nc )  transpose_square(f, nr);
	else
	{
		const ulong n1 = nr * nc - 1;
		const ulong ldnc = ld(nc);
		const ulong ldn = ld(n1+1);

		bitarray *tba = 0;
		if ( 0==ba )  tba = new bitarray(n1);
		else          tba = ba;
		tba->clear_all();

		for (ulong k=0;  k<n1;  k=tba->next_clear(++k) )  //  0 and n1 are fixed points
		{
			// do a cycle:
			ulong ks = SRC(k);
			ulong kd = k;
			tba->set(kd);
			Type t = f[kd];
			while ( ks != k )
			{
				f[kd] = f[ks];
				kd = ks;
				tba->set(kd);
				ks = SRC(ks);
			}
			f[kd] = t;
		}

		if ( 0==ba )  delete tba;
	}
}
#undef SRC
#undef DST 

static inline void zip(REAL *f, long n)
//
// lower half --> even indices
// higher half --> odd indices
//
// same as transposing the array as 2 x n/2 - matrix
//
// useful to combine real/imag part into a Complex array
// called by real_imag_to_complex()
{
	revbin_permute(f, n);
	revbin_permute((Complex *)f, n/2);
}
// -------------------------


static inline void unzip(REAL *f, long n)
//
// inverse of zip():
// put part of data with even indices
// sorted into the lower half,
// odd part into the higher half
//
// same as transposing the array as n/2 x 2 - matrix
//
// useful to separate real/imag part from a Complex array
// called by complex_to_real_imag()
{
	revbin_permute((Complex *)f, n/2);
	revbin_permute(f, n);
} 

static inline void cmult(REAL c, REAL s, REAL &u, REAL &v)
{ REAL t = u*s+v*c;  u *= c;  u -= v*s;  v = t; } 

static inline void ri_multiply(const REAL *fr, const REAL *fi,
			REAL *gr, REAL *gi, ulong n)
{
	while ( n-- )  cmult(fr[n], fi[n], gr[n], gi[n]);
}

template <typename Type> 
static inline void negate(Type *f, ulong n)
{
	while ( n-- )  
		f[n] = -f[n];
}

static inline void
csqr_n( REAL &u, REAL &v, REAL dn )
{ REAL t = u * u - v * v;  v *= ( u + u );  v *= dn;  u = t * dn; } 

static inline void
cmult_n( REAL c, REAL s, REAL &u, REAL &v, REAL dn)
{ REAL t = u * s + v * c;  u *= c;  u -= v * s;  u *= dn;  v = t * dn; }

#endif
