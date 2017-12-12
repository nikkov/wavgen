#ifndef __COMMON_CORE_DSP__
#define __COMMON_CORE_DSP__

#include "export.h"


//! Абстрактный класс менеджера распределения памяти, определяющий интерфейс менеджера
/*!
	Данный класс описывает 2 интерфейсные шаблонные функции менеджера
	распределения памяти Alloc и Free
*/
class COREDSP_API DSPMemoryAllocator
{
//! Функция реального выделения памяти
/*!
	Выделение памяти размером size, реализуемое в производном классе
	\param size	размер выделяемой памяти
	\return указатель на выделенную область памяти
*/
	virtual void *AllocInternal( ulong size ) = 0;

//! Функция реального освобождения памяти
/*!
	Освобождение памяти по указателю ptr, реализуемое в производном классе
	\param ptr	указатель на освобождаемую память
*/
	virtual void FreeInternal( void * ptr ) = 0;
public:
	DSPMemoryAllocator() {}
	virtual ~DSPMemoryAllocator() {}

//! Выделение памяти для size элементов типа T
/*!
	Выделение памяти для size элементов типа T
	\param size	число элементов под которые выделяется память
	\return указатель на выделенную область памяти
*/
	template <class T> 
	T* Alloc( ulong size )
	{
		return (T*)AllocInternal( size*sizeof(T) );
	}

//! Освобождение памяти ранее выделенной менеджером
/*!
	Освобождение памяти ранее выделенной менеджером
	\param ptr	указатель на освобождаемую область памяти
*/
	template <class T>
	void Free( T* ptr )
	{
		FreeInternal( (void*)ptr );
	}
};

//! Менеджер памяти, по умолчанию используемый библиотекой, если не указан другой
COREDSP_API extern DSPMemoryAllocator *defaultAllocator;

#endif //__COMMON_CORE_DSP__