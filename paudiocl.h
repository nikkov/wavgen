/////////////////////////////////////////////////////////////////////////////
// Name:        sinegen.h
// Purpose:     
// Author:      Nik
// Modified by: 
// Created:     10/07/2012 17:11:53
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PAUDIOCL_H_
#define _PAUDIOCL_H_


/*!
 * Includes
 */

////@begin includes
#include <iostream>
#include "portaudiocpp/PortAudioCpp.hxx"

#ifdef WIN32
#include "portaudiocpp/AsioDeviceAdapter.hxx"
#endif
////@end includes


class PaudioCl
{
public:
	PaudioCl();
	~PaudioCl();

	int GetDevicesNum(bool inputDirection);
	portaudio::Device& GetDevice(bool inputDirection, int index);
};


#endif
    // _PAUDIOCL_H_
