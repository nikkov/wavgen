#ifndef __HILBERT_FFT_TRANSFORM__
#define __HILBERT_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_hilbert
{
//! Вычисление аналитического сигнала через преобразование Фурье
/*!
	Вычисление аналитического сигнала вещественного ряда через 
	преобразование Фурье, на выходе комплексный сигнал, той же длины
\param src	входной первый массив
\param csrc	выходной массив, может совпадать с исходным массивом, 
	но для этого длина исходного массива должна быть в два раза больше, 
	чем длина исходных данных
\param n	длина массива входных данных. Должна быть четной и n/2 должна быть 
	допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_calc_analytic_signal( REAL *src, Complex *csrc, ulong n, namespace_fft::fftProc *fft_func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestHilbert();
#endif

}
#endif //__HILBERT_FFT_TRANSFORM__