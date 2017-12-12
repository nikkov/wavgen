#ifndef __UTILFUNC_WINDOW__
#define __UTILFUNC_WINDOW_

#include "complstd.h"
#include "commdsp.h"

namespace namespace_utilfunc
{
	enum WindowFuncType
	{
		WND_HANNING = 0,
		WND_HAMMING,
		WND_BLACKMAN,
		WND_BARTLETT,
		WND_KAISER,
		WND_BMAN_HARRIS,
		WND_RECTANGLE,
		WND_FLAT_TOP
	};


//! ������� ������� ��� ������� ������������ �����
/*!
	������� ��������� � ��������� ������� �������� ������� �������.
	\param data	�������/�������� ������ ������������ ������
	\param windowType	��� ������� �������
	\param coeff		�������������� �����������
	\param length		����� ��������/��������� ������� ������������ ������
*/
COREDSP_API void window_func_real ( REAL* data, const enum WindowFuncType windowType, REAL coeff, ulong length );

//! ������� ������� ��� ������� ����������� �����
/*!
	������� ��������� � ��������� ������� �������� ������� �������.
	\param data	�������/�������� ������ ����������� ������
	\param windowType	��� ������� �������
	\param coeff		�������������� �����������
	\param length		����� ��������/��������� ������� ������������ ������
*/
COREDSP_API void window_func_complex ( Complex* data, const enum WindowFuncType windowType, REAL coeff, ulong length );

}

#endif //__UTILFUNC_WINDOW_
