#ifndef __SINGLETONFFT_TRANSFORM__
#define __SINGLETONFFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"

namespace namespace_fft
{

//! �������� ������������ ����� ��� ��������� mixed radix fft by Singleton
/*!
	����������� ������������ ����� ��� ��������� mixed radix fft by Singleton
\param n	����������� �����
\return		true - ���� ����� �������� ���������� ��� ������� ���������
*/
COREDSP_API bool IsSingFFTLength( ulong n );

//! ��������� ���������� ����� ������� ��� ��������� mixed radix fft by Singleton
/*!
	������������� ���������� ����� ������� ��� ��������� mixed radix fft by Singleton.
\param n	��������������� �����
\param isign	1 ��� ��������� ����� ������� ��� ������ ���������������,
-1 ��� ��������� ����� ������� ��� ������ ���������������
\return		���������� ��� ������� ��������� ����� �������
*/
COREDSP_API ulong GetSingFftLength( ulong n, int isign );

//! ������� �������������� ����� (mixed radix fft by Singleton)
/*!
	������� �������������� ����� (mixed radix fft by Singleton).
	��� ������� �������������� ������������ ����� ������������������ 
	������������ ���������� ���� CmpException.
	��� ������� ��������� ��������� �������������� ������ � �������,
	��������� �� ����� �������. ��� ������ ���������� ����������
	��������� ������ � ������� ~2� �� �����.
\param f	�������/�������� ������ ����������� ������
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
\see DSPMemoryAllocator
*/
COREDSP_API void SingFFTStatic( Complex *f, ulong n, int isign );

//! ������� �������������� ����� (mixed radix fft by Singleton)
/*!
	������� �������������� ����� (mixed radix fft by Singleton).
	��� ������� �������������� ������������ ����� ������������������ 
	������������ ���������� ���� CmpException.
	��� ������� ��������� ��������� �������������� ������ � �������,
	������� ������� �� ����� �������. ��� ���������/������������ ������
	������������ �������� ����� ���������� ��� 
	(DSPMemoryAllocator *defaultAllocator).
\param f	�������/�������� ������ ����������� ������.
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
\see DSPMemoryAllocator
*/
COREDSP_API void SingFFTDynamic( Complex *f, ulong n, int isign );

//! ������� �������������� ����� (mixed radix fft by Singleton)
/*!
	������� �������������� ����� (mixed radix fft by Singleton).
	��� ������� �������������� ������������ ����� ������������������ 
	������������ ���������� ���� CmpException.
	��� ������� ��������� ��������� �������������� ������ � �������,
	������� ������� �� ����� �������. ��� ���������/������������ ������
	������������ ����������� �������������� ������ ���� DSPMemoryAllocator.
\param f	�������/�������� ������ ����������� ������
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
\param alloc	�������������� ������
\see DSPMemoryAllocator
*/
COREDSP_API void SingFFTDynamicAlloc( Complex *f, ulong n, int isign, DSPMemoryAllocator *alloc );

#ifdef USE_TEST_PROC
COREDSP_API bool TestSingFft();
#endif
}//namespace namespace_fft


#endif //__SINGLETONFFT_TRANSFORM__
