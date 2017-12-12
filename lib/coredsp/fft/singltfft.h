#ifndef __SINGLETONFFT_TRANSFORM__
#define __SINGLETONFFT_TRANSFORM__

#include "complstd.h"
#include "commdsp.h"

namespace namespace_fft
{

//! Проверка допустимости длины для алгоритма mixed radix fft by Singleton
/*!
	Проверяется допустимость длины для алгоритма mixed radix fft by Singleton
\param n	проверяемая длина
\return		true - если длина является допустимой для данного алгоритма
*/
COREDSP_API bool IsSingFFTLength( ulong n );

//! Получение допустимой длины массива для алгоритма mixed radix fft by Singleton
/*!
	Запрашивается допустимая длина массива для алгоритма mixed radix fft by Singleton.
\param n	ориентировочная длина
\param isign	1 для получения длины большей или равной ориентировочной,
-1 для получения длины меньшей или равной ориентировочной
\return		допустимая для данного алгоритма длина массива
*/
COREDSP_API ulong GetSingFftLength( ulong n, int isign );

//! Быстрое преобразование Фурье (mixed radix fft by Singleton)
/*!
	Быстрое преобразование Фурье (mixed radix fft by Singleton).
	При попытке преобразования недопустимой длины последовательности 
	генерируется исключение типа CmpException.
	Для данного алгоритма требуется дополнительная память в размере,
	зависящем от длины массива. Эта версия использует размещение
	требуемой памяти в размере ~2к на стеке.
\param f	входной/выходной массив комплексных данных
\param n	длина массива
\param isign	направление преобразования (-1 соответствует прямому, 1 - обратному БПФ )
\see DSPMemoryAllocator
*/
COREDSP_API void SingFFTStatic( Complex *f, ulong n, int isign );

//! Быстрое преобразование Фурье (mixed radix fft by Singleton)
/*!
	Быстрое преобразование Фурье (mixed radix fft by Singleton).
	При попытке преобразования недопустимой длины последовательности 
	генерируется исключение типа CmpException.
	Для данного алгоритма требуется дополнительная память в размере,
	который зависит от длины массива. Для выделения/освобождения памяти
	используется средства самой библиотеки ЦОС 
	(DSPMemoryAllocator *defaultAllocator).
\param f	входной/выходной массив комплексных данных.
\param n	длина массива
\param isign	направление преобразования (-1 соответствует прямому, 1 - обратному БПФ )
\see DSPMemoryAllocator
*/
COREDSP_API void SingFFTDynamic( Complex *f, ulong n, int isign );

//! Быстрое преобразование Фурье (mixed radix fft by Singleton)
/*!
	Быстрое преобразование Фурье (mixed radix fft by Singleton).
	При попытке преобразования недопустимой длины последовательности 
	генерируется исключение типа CmpException.
	Для данного алгоритма требуется дополнительная память в размере,
	который зависит от длины массива. Для выделения/освобождения памяти
	используется специальный распределитель памяти типа DSPMemoryAllocator.
\param f	входной/выходной массив комплексных данных
\param n	длина массива
\param isign	направление преобразования (-1 соответствует прямому, 1 - обратному БПФ )
\param alloc	распределитель памяти
\see DSPMemoryAllocator
*/
COREDSP_API void SingFFTDynamicAlloc( Complex *f, ulong n, int isign, DSPMemoryAllocator *alloc );

#ifdef USE_TEST_PROC
COREDSP_API bool TestSingFft();
#endif
}//namespace namespace_fft


#endif //__SINGLETONFFT_TRANSFORM__
