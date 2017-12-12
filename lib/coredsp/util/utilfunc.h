#ifndef __UTILFUNC_FFT_TRANSFORM__
#define __UTILFUNC_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_utilfunc
{
//! ������� �������������� ������� ������������ �����
/*!
	������� �������������� ������� ������������ ����� ����� ���.
	��� ������� �������������� ������������, ��� ��������� ������� ���
	�����, ������������������ ������������ ���������� ���� CmpException.
	\param x	�������/�������� ������ ������������ ������
	\param n	����� �������
	\param freq	������� �������������
	\param fft_func	������� ���, ������������ ��� ��������������
*/
COREDSP_API void fft_integral_real( REAL *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! ������� �������������� ������� ����������� �����
/*!
	������� �������������� ������� ����������� ����� ����� ���.
	��� ������� �������������� ������������, ��� ��������� ������� ���
	�����, ������������������ ������������ ���������� ���� CmpException.
	\param src	������� ������ ����������� ������
	\param n	����� �������
	\param freq	������� �������������
	\param fft_func	������� ���, ������������ ��� ��������������
*/
COREDSP_API void fft_integral_complex( Complex *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! ������� ����������������� ������� ������������ �����
/*!
	������� ����������������� ������� ������������ ����� ����� ���.
	��� ������� �������������� ������������, ��� ��������� ������� ���
	�����, ������������������ ������������ ���������� ���� CmpException.
	\param x	�������/�������� ������ ������������ ������
	\param n	����� �������
	\param freq	������� �������������
	\param fft_func	������� ���, ������������ ��� �����������������
*/
COREDSP_API void fft_differen_real( REAL *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! ������� ����������������� ������� ����������� �����
/*!
	������� ����������������� ������� ����������� ����� ����� ���.
	��� ������� �������������� ������������, ��� ��������� ������� ���
	�����, ������������������ ������������ ���������� ���� CmpException.
	\param src	������� ������ ����������� ������
	\param n	����� �������
	\param freq	������� �������������
	\param fft_func	������� ���, ������������ ��� �����������������
*/
COREDSP_API void fft_differen_complex( Complex *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! ������� �������� ��������� ������ �� ������� ������������ �����
/*!
	������� �������� ��������� ������ �� ������� ������������ �����.
	\param src	�������/�������� ������ ������������ ������
	\param len	����� �������
*/
COREDSP_API void RemoveLineTrend( REAL *src, ulong len );

//! ������� �������� ��������� ������ �� ������� ����������� �����
/*!
	������� �������� ��������� ������ �� ������� ����������� �����.
	\param src	�������/�������� ������ ����������� ������
	\param len	����� �������
*/
COREDSP_API void RemoveLineTrend( Complex *src, ulong len );


#ifdef USE_TEST_PROC
COREDSP_API bool TestIntegrDiff();
#endif

}//namespace_utilfunc

#endif //__UTILFUNC_FFT_TRANSFORM__
