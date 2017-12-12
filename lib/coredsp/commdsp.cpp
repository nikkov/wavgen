#include "commdspint.h"


SimpleDSPMemoryAllocator::SimpleDSPMemoryAllocator( ulong max_buffers_num )
{
	max_buffers = max_buffers_num;
	std::map<void*,MemoryPointer*>::const_iterator cIter;
	for ( cIter = poolMemory.begin(); cIter != poolMemory.end(); cIter++ )
	{
		task::sleep(100);
	}
}

SimpleDSPMemoryAllocator::~SimpleDSPMemoryAllocator()
{
	std::map<void*,MemoryPointer*>::const_iterator cIter;
	for ( cIter = poolMemory.begin(); cIter != poolMemory.end(); cIter++ )
	{
		ulong times = 10;
		while( cIter->second->IsLocked() && times-- )
			task::sleep(100);
		delete cIter->second->Ptr();
	}
	poolMemory.clear();
}

SimpleDSPMemoryAllocator::MemoryPointer* SimpleDSPMemoryAllocator::AllocElem( ulong size )
{
	MemoryPointer *elem = new MemoryPointer( size );
	poolMemory[elem->Ptr()] = elem;
	return elem;
}

SimpleDSPMemoryAllocator::MemoryPointer* SimpleDSPMemoryAllocator::FindElem( ulong size )
{
	std::map<void*,MemoryPointer*>::const_iterator cIter;

	MemoryPointer* ptr = NULL;

	// найдем свободный указатель нужного размера
	ulong max_size = 0;
	ulong free_counter = 0;
	for ( cIter = poolMemory.begin(); cIter != poolMemory.end(); cIter++ )
	{
		MemoryPointer* c_ptr = cIter->second;
		if( !c_ptr->IsLocked() )
		{
			if( c_ptr->Size() >= size &&
				( max_size == 0 || c_ptr->Size() < max_size ) )
			{
				max_size = c_ptr->Size();
				ptr = c_ptr;
			}
			free_counter++;
		}
	}
	// найдено
	if( ptr )
		return ptr;
	// не найдено, но есть свободные
	if( free_counter )
		for ( cIter = poolMemory.begin(); cIter != poolMemory.end(); cIter++ )
		{
			MemoryPointer* c_ptr = cIter->second;
			if( !c_ptr->IsLocked() )
			{
				poolMemory.erase( c_ptr->Ptr() );
				c_ptr->Resize( size );
				poolMemory[c_ptr->Ptr()] = c_ptr;
				return c_ptr;
			}
		}
	return NULL;
}

SimpleDSPMemoryAllocator::MemoryPointer* SimpleDSPMemoryAllocator::GetElem( ulong size )
{
	if( !poolMemory.size() )
		return AllocElem( size );

	MemoryPointer* ptr = FindElem( size );
	if( ptr )
		return ptr;
	// свободных нет, но можно выделить еще
	if ( !ptr && poolMemory.size() < max_buffers )
		return AllocElem( size );

	while( !ptr )
	{
		ptr = FindElem( size );
		if( !ptr )
			task::sleep( 100 );
	}
	if( !ptr )
		THROW_EXCEPTION2( "Can't allocate memory for size %u", size );
	return ptr;
}

void *SimpleDSPMemoryAllocator::AllocInternal( ulong size )
{
	critical_section crit( guard );
	MemoryPointer* ptr = GetElem( size );
	if( !ptr )
		THROW_EXCEPTION2( "Can't allocate memory for size %u", size );
	ptr->Lock();
	return ptr->Ptr();
}

void SimpleDSPMemoryAllocator::FreeInternal( void * ptr )
{
	critical_section crit( guard );
	std::map<void*,MemoryPointer*>::const_iterator cIter;
	cIter = poolMemory.find(ptr);
	if( cIter == poolMemory.end() )
		THROW_EXCEPTION2( "Can't free memory for pointer %u", (ulong)ptr );
	cIter->second->Unlock();
}
