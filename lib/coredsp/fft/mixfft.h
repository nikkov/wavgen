#ifndef __MIXEDFFT_TRANSFORM__
#define __MIXEDFFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"

namespace namespace_fft
{
//! �������� ������������ ����� ��� ����������������� ��������� mixed radix fft
/*!
	����������� ������������ ����� ��� ����������������� ��������� mixed radix fft.
\param n	����������� �����
\return		true - ���� ����� �������� ���������� ��� ������� ���������
*/
COREDSP_API bool IsMixRadixFftLength( ulong n );

//! ��������� ���������� ����� ������� ��� ����������������� ��������� mixed radix fft
/*!
	������������� ���������� ����� ������� ��� ����������������� ��������� mixed radix fft.
\param n	��������������� �����
\param isign	1 ��� ��������� ����� ������� ��� ������ ���������������,
-1 ��� ��������� ����� ������� ��� ������ ���������������
\return		���������� ��� ������� ��������� ����� �������
*/
COREDSP_API ulong GetMixRadixFftLength( ulong n, int isign );

//! ������� �������������� ����� (���������������� mixed radix fft)
/*!
	������� �������������� ����� (���������������� mixed radix fft).
	��� ������� �������������� ������������ ����� ������������������ 
	������������ ���������� ���� CmpException.
\param x	������� ������ ����������� ������
\param y	�������� ������ ����������� ������
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
*/
COREDSP_API void MixRadixFft( Complex *x, Complex *y, ulong n, int isign );

//! ������� �������������� ����� (���������������� mixed radix fft)
/*!
	������� �������������� ����� (���������������� mixed radix fft).
	��� ������� �������������� ������������ ����� ������������������ 
	������������ ���������� ���� CmpException.
	��� ������� ��������� ��������� �������������� ������ � �������
	������ ������� ��������� �������. ��� ���������/������������ ������
	������������ ����������� �������������� ������ ���� DSPMemoryAllocator.
\param x	�������/�������� ������ ����������� ������
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
\param alloc	�������������� ������
\see DSPMemoryAllocator
*/
COREDSP_API void MixRadixFft( Complex *x, ulong n, int isign, DSPMemoryAllocator *alloc );

//! ������� �������������� ����� (���������������� mixed radix fft)
/*!
	������� �������������� ����� (���������������� mixed radix fft).
	��� ������� �������������� ������������ ����� ������������������ 
	������������ ���������� ���� CmpException.
	��� ������� ��������� ��������� �������������� ������ � �������
	������ ������� ��������� �������. ��� ���������/������������ ������
	������������ �������� ����� ���������� ��� (DSPMemoryAllocator *defaultAllocator).
\param x	�������/�������� ������ ����������� ������
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
\see DSPMemoryAllocator
*/
COREDSP_API void MixRadixFft( Complex *x, ulong n, int isign );

}//namespace namespace_fft

#ifdef USE_TEST_PROC
COREDSP_API bool TestMixFft();
#endif
#endif //__MIXEDFFT_TRANSFORM__
