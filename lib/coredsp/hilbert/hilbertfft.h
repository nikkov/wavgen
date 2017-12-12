#ifndef __HILBERT_FFT_TRANSFORM__
#define __HILBERT_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_hilbert
{
//! ���������� �������������� ������� ����� �������������� �����
/*!
	���������� �������������� ������� ������������� ���� ����� 
	�������������� �����, �� ������ ����������� ������, ��� �� �����
\param src	������� ������ ������
\param csrc	�������� ������, ����� ��������� � �������� ��������, 
	�� ��� ����� ����� ��������� ������� ������ ���� � ��� ���� ������, 
	��� ����� �������� ������
\param n	����� ������� ������� ������. ������ ���� ������ � n/2 ������ ���� 
	���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_calc_analytic_signal( REAL *src, Complex *csrc, ulong n, namespace_fft::fftProc *fft_func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestHilbert();
#endif

}
#endif //__HILBERT_FFT_TRANSFORM__