#ifndef __REALFFT_TRANSFORM__
#define __REALFFT_TRANSFORM__

#include "complstd.h"

namespace namespace_fft
{

//! ��� - ��������� �� ������� ���
/*!
	����������� ���� ��������� �� ������� ���
*/
typedef void  fftProc( Complex*, ulong, int );

//! ������� �������������� ����� ��� ������������ �����
/*!
	������� �������������� ����� ��� ������������ �����, ������������ � ��� ��
	������, ��� ����������� �������� ������. ��� ������� �������������� ������������
	����� ������������������ ������������ ���������� ���� CmpException.

	������� �������� ������������������:

	 x[0]     = c.re[0]   (���������� ������������, ������������ ��������)
	 x[1]     = c.re[n/2] (������������ �� ������� ���������, ������������ ��������)
	 x[2]     = c.re[1]
	 x[3]     = c.im[1]
	 x[4]     = c.re[2]
	 x[5]     = c.im[2]
	         ...
	 x[2*i]   = c.re[i]
	 x[2*i+1] = c.im[i]
	         ...
	 x[n-2]   = c.re[n/2-1]
	 x[n-1]   = c.im[n/2-1] 

\param x	������� REAL/�������� Complex ������ ������ ��� ������� ��������������
			������� Complex/�������� REAL ������ ������ ��� ��������� ��������������
\param n	����� ������� ������������ ������, ������ ���� ������, 
			�������� ����������� ������ ����� ����� n/2
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
\param func	 ������� ���� fftProc ��� �������������� ������������ ������� ������ n/2
				���� func = NULL, �� ������������ ������� PfaFft
*/
COREDSP_API void RealFFt( REAL *x, ulong n, int isign, fftProc* func = NULL );

#ifdef USE_TEST_PROC
COREDSP_API bool TestRealFft();
#endif
}//namespace namespace_fft


#endif //__REALFFT_TRANSFORM__
