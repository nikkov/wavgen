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


//! Оконная функция для массива вещественных чисел
/*!
	Функция применяет к исходному массиву заданную оконную функцию.
	\param data	входной/выходной массив вещественных данных
	\param windowType	тип оконной функции
	\param coeff		масштабирующий коэффициент
	\param length		длина входного/выходного массива вещественных данных
*/
COREDSP_API void window_func_real ( REAL* data, const enum WindowFuncType windowType, REAL coeff, ulong length );

//! Оконная функция для массива комплексных чисел
/*!
	Функция применяет к исходному массиву заданную оконную функцию.
	\param data	входной/выходной массив комплексных данных
	\param windowType	тип оконной функции
	\param coeff		масштабирующий коэффициент
	\param length		длина входного/выходного массива вещественных данных
*/
COREDSP_API void window_func_complex ( Complex* data, const enum WindowFuncType windowType, REAL coeff, ulong length );

}

#endif //__UTILFUNC_WINDOW_
