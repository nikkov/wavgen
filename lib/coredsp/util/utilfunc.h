#ifndef __UTILFUNC_FFT_TRANSFORM__
#define __UTILFUNC_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_utilfunc
{
//! Функция интегрирования массива вещественных чисел
/*!
	Функция интегрирования массива вещественных чисел через БПФ.
	При попытке преобразования недопустимой, для указанной функции БПФ
	длины, последовательности генерируется исключение типа CmpException.
	\param x	входной/выходной массив вещественных данных
	\param n	длина массива
	\param freq	частота дискретизации
	\param fft_func	функция БПФ, используемая при интегрировании
*/
COREDSP_API void fft_integral_real( REAL *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! Функция интегрирования массива комплексных чисел
/*!
	Функция интегрирования массива комплексных чисел через БПФ.
	При попытке преобразования недопустимой, для указанной функции БПФ
	длины, последовательности генерируется исключение типа CmpException.
	\param src	входной массив комплексных данных
	\param n	длина массива
	\param freq	частота дискретизации
	\param fft_func	функция БПФ, используемая при интегрировании
*/
COREDSP_API void fft_integral_complex( Complex *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! Функция дифференцирования массива вещественных чисел
/*!
	Функция дифференцирования массива вещественных чисел через БПФ.
	При попытке преобразования недопустимой, для указанной функции БПФ
	длины, последовательности генерируется исключение типа CmpException.
	\param x	входной/выходной массив вещественных данных
	\param n	длина массива
	\param freq	частота дискретизации
	\param fft_func	функция БПФ, используемая при дифференцировании
*/
COREDSP_API void fft_differen_real( REAL *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! Функция дифференцирования массива комплексных чисел
/*!
	Функция дифференцирования массива комплексных чисел через БПФ.
	При попытке преобразования недопустимой, для указанной функции БПФ
	длины, последовательности генерируется исключение типа CmpException.
	\param src	входной массив комплексных данных
	\param n	длина массива
	\param freq	частота дискретизации
	\param fft_func	функция БПФ, используемая при дифференцирования
*/
COREDSP_API void fft_differen_complex( Complex *src, ulong n, double freq, namespace_fft::fftProc *fft_func );

//! Функция удаления линейного тренда из массива вещественных чисел
/*!
	Функция удаления линейного тренда из массива вещественных чисел.
	\param src	входной/выходной массив вещественных данных
	\param len	длина массива
*/
COREDSP_API void RemoveLineTrend( REAL *src, ulong len );

//! Функция удаления линейного тренда из массива комплексных чисел
/*!
	Функция удаления линейного тренда из массива комплексных чисел.
	\param src	входной/выходной массив комплексных данных
	\param len	длина массива
*/
COREDSP_API void RemoveLineTrend( Complex *src, ulong len );


#ifdef USE_TEST_PROC
COREDSP_API bool TestIntegrDiff();
#endif

}//namespace_utilfunc

#endif //__UTILFUNC_FFT_TRANSFORM__
