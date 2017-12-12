#ifndef __CORRELATION_FFT_TRANSFORM__
#define __CORRELATION_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_correlation
{

//! Вычисление циклической взаимной корреляции через преобразование Фурье
/*!
	Вычисление циклической взаимной корреляции вещественных рядов через 
	преобразование Фурье
\param f	входной первый массив
\param g	входной второй массив/выходной массив
\param n	длина массива. Должна быть четной и n/2 должна быть 
			допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_correlation( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *fft_func );

//! Вычисление циклической автокорреляции через преобразование Фурье
/*!
	Вычисление циклической автокорреляции вещественного ряда через 
	преобразование Фурье
\param f	входной/выходной массив
\param n	длина массива. Должна быть четной и n/2 должна быть 
			допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_auto_correlation( REAL *f, ulong n, namespace_fft::fftProc *fft_func );

//! Вычисление циклической взаимной корреляции через преобразование Фурье
/*!
	Вычисление циклической взаимной корреляции комплексных рядов через 
	преобразование Фурье
\param f	входной первый массив
\param g	входной второй массив/выходной массив
\param n	длина массивов. Должна быть допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_complex_correlation( Complex* f, Complex* g, ulong n, namespace_fft::fftProc *fft_func );

//! Вычисление циклической автокорреляции через преобразование Фурье
/*!
	Вычисление циклической автокорреляции комплексных ряда через 
	преобразование Фурье
\param f	входной/выходной массив
\param n	длина массива. Должна быть допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_complex_auto_correlation( Complex *f, ulong n, namespace_fft::fftProc *fft_func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestCorrelation();
#endif

} //namespace namespace_correlation


#endif //__CORRELATION_FFT_TRANSFORM__
