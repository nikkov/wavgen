#ifndef __CONVOLUTION_FFT_TRANSFORM__
#define __CONVOLUTION_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

#pragma message( "#Convolution function is not tested!#" )

namespace namespace_convolution
{
//! ���������� ����������� ����������� ����� �������������� �����
/*!
	���������� ����������� ����������� ����������� ����� ����� 
	�������������� �����
\param f	�������/�������� ������
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
	���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_complex_auto_convolution( Complex *f, ulong n, double v, namespace_fft::fftProc *func );

//! ���������� ����������� �������� ������� ����� �������������� �����
/*!
	���������� ����������� �������� ������� ����������� ����� ����� 
	�������������� �����
\param f	������� ������ ������
\param g	������� ������ ������/�������� ������
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
	���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_complex_convolution( Complex *f, Complex *g, ulong n, double v, namespace_fft::fftProc *func );

//! ���������� ����������� ����������� ����� �������������� �����
/*!
	���������� ����������� ����������� ������������ ����� ����� 
	�������������� �����
\param f	�������/�������� ������ ������
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
	���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_auto_convolution( REAL *f, ulong n, namespace_fft::fftProc *func );

//! ���������� ����������� �������� ������� ����� �������������� �����
/*!
	���������� ����������� �������� ������� ������������ ����� ����� 
	�������������� �����
\param f	������� ������ ������
\param g	������� ������ ������/�������� ������
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
	���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void fft_convolution( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestConvolution();
#endif

}//namespace namespace_convolution

#endif //__CONVOLUTION_FFT_TRANSFORM__