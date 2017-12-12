#include <assert.h>
#include "export.h"
#include "complstd.h"
#include "common/arith.h"

COREDSP_API void Complex2DoubleArray( Complex *c, long n )
//
// this routine transforms
// complex data into two seperate fields
// with real and imag data (in-place)
//
// n := array length
{
	unzip((double *)c, 2*n);
}
// -------------------------


COREDSP_API void Double2ComplexArray( double *fr, double *fi, long n )
//
// this routine transforms
// two seperate fields with real and imag
// data into complex data (in-place)
// (the data must lie in contiguous memory)
//
// n := array length(s)
{
	assert( fi==fr+n ); // assert contiguous memory
	zip(fr, 2*n);
} 