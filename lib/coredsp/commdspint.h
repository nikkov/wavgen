#ifndef __COMMON_CORE_DSP_INTERNAL__
#define __COMMON_CORE_DSP_INTERNAL__

#include <map>
#include "sal/task.h"
#include "commdsp.h"
#include "dspexcept.h"


//! Класс простого менеджера распределения памяти
/*!
	Данный класс использует выделение блоков памяти из внутреннего пула.
	Обеспечивает синхронизацию операций в многопоточном окружении
*/
class SimpleDSPMemoryAllocator : public DSPMemoryAllocator
{
	//! Класс описывающий один блок памяти в пуле SimpleDSPMemoryAllocator
	class MemoryPointer
	{
		mutable mutex guard;
		mutable bool lock;
		void *ptr;
		ulong size;
	public:
		MemoryPointer( ulong new_size ) : ptr(NULL), size(0), lock(false)
		{
			Resize( new_size );
		}
		MemoryPointer( const MemoryPointer& other ) : ptr(NULL), size(0), lock(false)
		{
			Resize( other.size );
			memcpy( ptr, other.ptr, other.size );
		}
		~MemoryPointer()
		{
			if( ptr )
				delete ptr;
		}
		// Оператор копирования
		MemoryPointer& operator = ( const MemoryPointer& other )
		{
			Resize( other.size );
			memcpy( ptr, other.ptr, other.size );
			return *this;
		}

		void Resize( ulong new_size )
		{
#ifndef __NOT_USE_DOUBLER_SIZE__
			new_size *= 2;
#endif //__NOT_USE_DOUBLER_SIZE__

			if( ptr )
				delete ptr;
			try
			{
				ptr = new char[new_size];
				if( !ptr )
					throw;
			}
			catch(...)
			{
				THROW_EXCEPTION2( "Can't allocate memory for size %u", new_size );
			}

			size = new_size;
		}
		inline ulong Size() const
		{ return size; }
		inline void Lock() const
		{ guard.enter(); lock = true; guard.leave(); }
		inline void Unlock() const
		{ guard.enter(); lock = false; guard.leave(); }
		inline bool IsLocked() const
		{ guard.enter(); bool _lock = lock; guard.leave(); return _lock; }

		inline void* Ptr() const
		{ return ptr; }
	};
	mutex guard;
	ulong max_buffers;
	std::map<void*,MemoryPointer*> poolMemory;
	virtual void *AllocInternal( ulong size );
	virtual void FreeInternal(void *);

	MemoryPointer* AllocElem( ulong size );
	MemoryPointer* FindElem( ulong size );
	MemoryPointer* GetElem( ulong size );

public:
	SimpleDSPMemoryAllocator( ulong max_buffer );
	virtual ~SimpleDSPMemoryAllocator();
};


#endif //__COMMON_CORE_DSP_INTERNAL__
