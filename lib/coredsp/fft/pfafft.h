#ifndef __PFAFFT_TRANSFORM__
#define __PFAFFT_TRANSFORM__

#include "complstd.h"

namespace namespace_fft
{

//! �������� ������������ ����� ��� ��������� primary factor fft
/*!
	����������� ������������ ����� ��� ��������� primary factor fft.
\param n	����������� �����
\return		true - ���� ����� �������� ���������� ��� ������� ���������
*/
COREDSP_API bool IsPfaFftLength( ulong n );

//! ��������� ���������� ����� ������� ��� ��������� primary factor fft
/*!
	������������� ���������� ����� ������� ��� ��������� primary factor fft.
	\param n	��������������� �����
	\param isign	1 ��� ��������� ����� ������� ��� ������ ���������������,
					-1 ��� ��������� ����� ������� ��� ������ ���������������
	\return		���������� ��� ������� ��������� ����� �������
*/
COREDSP_API ulong GetPfaFftLength( ulong n, int isign );

//! ������� �������������� ����� (primary factor fft)
/*!
	������� �������������� ����� (primary factor fft), ������������ � ��� ��
	������, ��� ����������� �������� ������. ��� ������� �������������� ������������
	����� ������������������ ������������ ���������� ���� CmpException.
\param cz	�������/�������� ������ ����������� ������
\param n	����� �������
\param isign	����������� �������������� (-1 ������������� �������, 1 - ��������� ��� )
*/
COREDSP_API void PfaFft( Complex *cz, ulong n, int isign );

#ifdef USE_TEST_PROC
COREDSP_API bool TestPfaFft();
#endif
}//namespace namespace_fft

#endif //__PFAFFT_TRANSFORM__
