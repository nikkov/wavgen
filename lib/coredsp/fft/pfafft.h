#ifndef __PFAFFT_TRANSFORM__
#define __PFAFFT_TRANSFORM__

#include "complstd.h"

namespace namespace_fft
{

//! ѕроверка допустимости длины дл€ алгоритма primary factor fft
/*!
	ѕровер€етс€ допустимость длины дл€ алгоритма primary factor fft.
\param n	провер€ема€ длина
\return		true - если длина €вл€етс€ допустимой дл€ данного алгоритма
*/
COREDSP_API bool IsPfaFftLength( ulong n );

//! ѕолучение допустимой длины массива дл€ алгоритма primary factor fft
/*!
	«апрашиваетс€ допустима€ длина массива дл€ алгоритма primary factor fft.
	\param n	ориентировочна€ длина
	\param isign	1 дл€ получени€ длины большей или равной ориентировочной,
					-1 дл€ получени€ длины меньшей или равной ориентировочной
	\return		допустима€ дл€ данного алгоритма длина массива
*/
COREDSP_API ulong GetPfaFftLength( ulong n, int isign );

//! Ѕыстрое преобразование ‘урье (primary factor fft)
/*!
	Ѕыстрое преобразование ‘урье (primary factor fft), производимое в той же
	пам€ти, где расположены исходные данные. ѕри попытке преобразовани€ недопустимой
	длины последовательности генерируетс€ исключение типа CmpException.
\param cz	входной/выходной массив комплексных данных
\param n	длина массива
\param isign	направление преобразовани€ (-1 соответствует пр€мому, 1 - обратному Ѕѕ‘ )
*/
COREDSP_API void PfaFft( Complex *cz, ulong n, int isign );

#ifdef USE_TEST_PROC
COREDSP_API bool TestPfaFft();
#endif
}//namespace namespace_fft

#endif //__PFAFFT_TRANSFORM__
