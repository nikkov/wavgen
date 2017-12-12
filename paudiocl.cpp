#include "paudiocl.h"

using namespace portaudio;

PaudioCl::PaudioCl()
{
	if(!System::exists())
		System::initialize();
}

PaudioCl::~PaudioCl()
{
	if (System::exists())
		System::terminate();
}

int PaudioCl::GetDevicesNum(bool inputDirection)
{
	int retNum = 0;
	System &sys = System::instance();
	for (System::DeviceIterator i = sys.devicesBegin(); i != sys.devicesEnd(); ++i)  
	{
		if ((*i).isFullDuplexDevice() || (inputDirection ? (*i).isInputOnlyDevice() : (*i).isOutputOnlyDevice()))
			retNum++;
	}
	return retNum;
}

Device& PaudioCl::GetDevice(bool inputDirection, int index)
{
	System &sys = System::instance();
	for (System::DeviceIterator i = sys.devicesBegin(); i != sys.devicesEnd(); ++i)  
	{
		if ((*i).isFullDuplexDevice() || (inputDirection ? (*i).isInputOnlyDevice() : (*i).isOutputOnlyDevice()))
		{
			if(index == 0)
				return (*i);
			index--;
		}
	}
	throw PaException(paInvalidDevice);
}
