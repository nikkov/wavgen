#ifndef __CROSS_AND_AUTO_TRANSFORM__
#define __CROSS_AND_AUTO_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"
#include "fft/realfft.h"

namespace namespace_fft
{
//! Вычисление взаимного спектра
/*!
	Вычисление взаимного спектра комплексных рядов
\param f	входной первый массив
\param g	входной второй массив/выходной массив
\param n	длина массива. Должна быть допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void CrossPowerSpectrumComplex( Complex *f, Complex *g, ulong n, namespace_fft::fftProc *fft_func );

//! Вычисление собственного спектра
/*!
	Вычисление собственного комплексного ряда
\param f	входной массив/выходной массив
\param n	длина массива. Должна быть допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void AutoPowerSpectrumComplex( Complex *f, ulong n, namespace_fft::fftProc *fft_func );

//! Вычисление взаимного спектра
/*!
	Вычисление взаимного спектра вещественных рядов
\param f	входной первый массив
\param g	входной второй массив/выходной массив (комплексный)
\param n	длина массива. Должна быть четной и n/2 должна быть 
допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void CrossPowerSpectrumReal( REAL *f, REAL *g, ulong n, namespace_fft::fftProc *fft_func );

//! Вычисление собственного спектра
/*!
	Вычисление собственного вещественного ряда
\param f	входной массив/выходной массив (комплексный)
\param n	длина массива. Должна быть четной и n/2 должна быть 
допустимой для используемого алгоритма БПФ
\param fft_func	указатель на функцию БПФ
\see fftProc
*/
COREDSP_API void AutoPowerSpectrumReal( REAL *f, ulong n, namespace_fft::fftProc *fft_func );

#ifdef USE_TEST_PROC
COREDSP_API bool TestAutoCrossPowerSpectrum();
#endif
}

#endif //__CROSS_AND_AUTO_TRANSFORM__
