#ifndef __COMMON_CORE_DSP__
#define __COMMON_CORE_DSP__

#include "export.h"


//! ����������� ����� ��������� ������������� ������, ������������ ��������� ���������
/*!
	������ ����� ��������� 2 ������������ ��������� ������� ���������
	������������� ������ Alloc � Free
*/
class COREDSP_API DSPMemoryAllocator
{
//! ������� ��������� ��������� ������
/*!
	��������� ������ �������� size, ����������� � ����������� ������
	\param size	������ ���������� ������
	\return ��������� �� ���������� ������� ������
*/
	virtual void *AllocInternal( ulong size ) = 0;

//! ������� ��������� ������������ ������
/*!
	������������ ������ �� ��������� ptr, ����������� � ����������� ������
	\param ptr	��������� �� ������������� ������
*/
	virtual void FreeInternal( void * ptr ) = 0;
public:
	DSPMemoryAllocator() {}
	virtual ~DSPMemoryAllocator() {}

//! ��������� ������ ��� size ��������� ���� T
/*!
	��������� ������ ��� size ��������� ���� T
	\param size	����� ��������� ��� ������� ���������� ������
	\return ��������� �� ���������� ������� ������
*/
	template <class T> 
	T* Alloc( ulong size )
	{
		return (T*)AllocInternal( size*sizeof(T) );
	}

//! ������������ ������ ����� ���������� ����������
/*!
	������������ ������ ����� ���������� ����������
	\param ptr	��������� �� ������������� ������� ������
*/
	template <class T>
	void Free( T* ptr )
	{
		FreeInternal( (void*)ptr );
	}
};

//! �������� ������, �� ��������� ������������ �����������, ���� �� ������ ������
COREDSP_API extern DSPMemoryAllocator *defaultAllocator;

#endif //__COMMON_CORE_DSP__