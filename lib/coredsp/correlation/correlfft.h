#ifndef __CORRELATION_FFT_TRANSFORM__
#define __CORRELATION_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_correlation
{

//! ���������� ����������� �������� ���������� ����� �������������� �����
/*!
	���������� ����������� �������� ���������� ������������ ����� ����� 
	�������������� �����
\param f	������� ������ ������
\param g	������� ������ ������/�������� ������
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
			���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_correlation( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *fft_func );

//! ���������� ����������� �������������� ����� �������������� �����
/*!
	���������� ����������� �������������� ������������� ���� ����� 
	�������������� �����
\param f	�������/�������� ������
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
			���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_auto_correlation( REAL *f, ulong n, namespace_fft::fftProc *fft_func );

//! ���������� ����������� �������� ���������� ����� �������������� �����
/*!
	���������� ����������� �������� ���������� ����������� ����� ����� 
	�������������� �����
\param f	������� ������ ������
\param g	������� ������ ������/�������� ������
\param n	����� ��������. ������ ���� ���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_complex_correlation( Complex* f, Complex* g, ulong n, namespace_fft::fftProc *fft_func );

//! ���������� ����������� �������������� ����� �������������� �����
/*!
	���������� ����������� �������������� ����������� ���� ����� 
	�������������� �����
\param f	�������/�������� ������
\param n	����� �������. ������ ���� ���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_complex_auto_correlation( Complex *f, ulong n, namespace_fft::fftProc *fft_func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestCorrelation();
#endif

} //namespace namespace_correlation


#endif //__CORRELATION_FFT_TRANSFORM__
