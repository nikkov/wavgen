#ifndef __CROSS_AND_AUTO_TRANSFORM__
#define __CROSS_AND_AUTO_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_fft
{
//! ���������� ��������� �������
/*!
	���������� ��������� ������� ����������� �����
\param f	������� ������ ������
\param g	������� ������ ������/�������� ������
\param n	����� �������. ������ ���� ���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void CrossPowerSpectrumComplex( Complex *f, Complex *g, ulong n, namespace_fft::fftProc *fft_func );

//! ���������� ������������ �������
/*!
	���������� ������������ ������������ ����
\param f	������� ������/�������� ������
\param n	����� �������. ������ ���� ���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void AutoPowerSpectrumComplex( Complex *f, ulong n, namespace_fft::fftProc *fft_func );

//! ���������� ��������� �������
/*!
	���������� ��������� ������� ������������ �����
\param f	������� ������ ������
\param g	������� ������ ������/�������� ������ (�����������)
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void CrossPowerSpectrumReal( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *fft_func );

//! ���������� ������������ �������
/*!
	���������� ������������ ������������� ����
\param f	������� ������/�������� ������ (�����������)
\param n	����� �������. ������ ���� ������ � n/2 ������ ���� 
���������� ��� ������������� ��������� ���
\param fft_func	��������� �� ������� ���
\see fftProc
*/
COREDSP_API void AutoPowerSpectrumReal( REAL *f, ulong n, namespace_fft::fftProc *fft_func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestAutoCrossPowerSpectrum();
#endif
}

#endif //__CROSS_AND_AUTO_TRANSFORM__
