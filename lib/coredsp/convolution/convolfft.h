#ifndef __CONVOLUTION_FFT_TRANSFORM__
#define __CONVOLUTION_FFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

#pragma message( "#Convolution function is not tested!#" )

namespace namespace_convolution
{
//! Вычисление циклической автосвертки через преобразование Фурье
/*!
	Вычисление циклической автосвертки комплексных рядов через 
	преобразование Фурье
\param f	входной/выходной массив
\param n	длина массива. Должна быть четной и n/2 должна быть 
	допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_complex_auto_convolution( Complex *f, ulong n, double v, namespace_fft::fftProc *func );

//! Вычисление циклической взаимной свертки через преобразование Фурье
/*!
	Вычисление циклической взаимной свертки комплексных рядов через 
	преобразование Фурье
\param f	входной первый массив
\param g	входной второй массив/выходной массив
\param n	длина массива. Должна быть четной и n/2 должна быть 
	допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_complex_convolution( Complex *f, Complex *g, ulong n, double v, namespace_fft::fftProc *func );

//! Вычисление циклической автосвертки через преобразование Фурье
/*!
	Вычисление циклической автосвертки вещественных рядов через 
	преобразование Фурье
\param f	входной/выходной первый массив
\param n	длина массива. Должна быть четной и n/2 должна быть 
	допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_auto_convolution( REAL *f, ulong n, namespace_fft::fftProc *func );

//! Вычисление циклической взаимной свертки через преобразование Фурье
/*!
	Вычисление циклической взаимной свертки вещественных рядов через 
	преобразование Фурье
\param f	входной первый массив
\param g	входной второй массив/выходной массив
\param n	длина массива. Должна быть четной и n/2 должна быть 
	допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void fft_convolution( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestConvolution();
#endif

}//namespace namespace_convolution

#endif //__CONVOLUTION_FFT_TRANSFORM__