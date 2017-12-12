#ifndef __MIXEDFFT_TRANSFORM__
#define __MIXEDFFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"

namespace namespace_fft
{
//! Проверка допустимости длины для модифицированного алгоритма mixed radix fft
/*!
	Проверяется допустимость длины для модифицированного алгоритма mixed radix fft.
\param n	проверяемая длина
\return		true - если длина является допустимой для данного алгоритма
*/
COREDSP_API bool IsMixRadixFftLength( ulong n );

//! Получение допустимой длины массива для модифицированного алгоритма mixed radix fft
/*!
	Запрашивается допустимая длина массива для модифицированного алгоритма mixed radix fft.
\param n	ориентировочная длина
\param isign	1 для получения длины большей или равной ориентировочной,
-1 для получения длины меньшей или равной ориентировочной
\return		допустимая для данного алгоритма длина массива
*/
COREDSP_API ulong GetMixRadixFftLength( ulong n, int isign );

//! Быстрое преобразование Фурье (модифицированный mixed radix fft)
/*!
	Быстрое преобразование Фурье (модифицированный mixed radix fft).
	При попытке преобразования недопустимой длины последовательности 
	генерируется исключение типа CmpException.
\param x	входной массив комплексных данных
\param y	выходной массив комплексных данных
\param n	длина массива
\param isign	направление преобразования (-1 соответствует прямому, 1 - обратному БПФ )
*/
COREDSP_API void MixRadixFft( Complex *x, Complex *y, ulong n, int isign );

//! Быстрое преобразование Фурье (модифицированный mixed radix fft)
/*!
	Быстрое преобразование Фурье (модифицированный mixed radix fft).
	При попытке преобразования недопустимой длины последовательности 
	генерируется исключение типа CmpException.
	Для данного алгоритма требуется дополнительная память в размере
	равном размеру исходного массива. Для выделения/освобождения памяти
	используется специальный распределитель памяти типа DSPMemoryAllocator.
\param x	входной/выходной массив комплексных данных
\param n	длина массива
\param isign	направление преобразования (-1 соответствует прямому, 1 - обратному БПФ )
\param alloc	распределитель памяти
\see DSPMemoryAllocator
*/
COREDSP_API void MixRadixFft( Complex *x, ulong n, int isign, DSPMemoryAllocator *alloc );

//! Быстрое преобразование Фурье (модифицированный mixed radix fft)
/*!
	Быстрое преобразование Фурье (модифицированный mixed radix fft).
	При попытке преобразования недопустимой длины последовательности 
	генерируется исключение типа CmpException.
	Для данного алгоритма требуется дополнительная память в размере
	равном размеру исходного массива. Для выделения/освобождения памяти
	используется средства самой библиотеки ЦОС (DSPMemoryAllocator *defaultAllocator).
\param x	входной/выходной массив комплексных данных
\param n	длина массива
\param isign	направление преобразования (-1 соответствует прямому, 1 - обратному БПФ )
\see DSPMemoryAllocator
*/
COREDSP_API void MixRadixFft( Complex *x, ulong n, int isign );

}//namespace namespace_fft

#ifdef USE_TEST_PROC
COREDSP_API bool TestMixFft();
#endif
#endif //__MIXEDFFT_TRANSFORM__
