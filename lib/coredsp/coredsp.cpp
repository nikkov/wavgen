// CoreDSP.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include "CoreDSP.h"
#include "commdspint.h"

COREDSP_API DSPMemoryAllocator *defaultAllocator = NULL;

class internalStaticSingleton
{
	mutex guard;
public:
	internalStaticSingleton() 
	{
		critical_section crit( guard );
		if( !defaultAllocator )
			defaultAllocator = new SimpleDSPMemoryAllocator( 5 );
	}
	~internalStaticSingleton() 
	{
		critical_section crit( guard );
		if( defaultAllocator )
		{
			delete defaultAllocator;
			defaultAllocator = NULL;
		}
	}
};


internalStaticSingleton singleton;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

