#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <math.h>
#include "Wave.h"


CWave::CWave(void)
{
	// Init members
	memset(&m_Descriptor, 0, sizeof(_WAVEDESCR));
	memset(&m_Format, 0, sizeof(_WAVEFORMAT));
	m_lpData = NULL;
	m_dwSize = 0;
	memset(&m_WaveHeader, 0, sizeof(WAVEHDR));

	memcpy(m_Descriptor.riff, "RIFF", 4);
	memcpy(m_Descriptor.wave, "WAVE", 4);
	memcpy(m_Format.id, "fmt ", 4);
	m_Format.size = 16;
	m_Format.format = 0x0001;//PCM
	m_Format.channels = 2;
}

CWave::~CWave(void)
{
}

bool CWave::Save(wxString& filePath)
{
	bool bResult = FALSE;

	// Save .WAV file
	FILE* file = _tfopen(filePath, _T("wb"));
	if (file != NULL)
	{
		// Save .WAV descriptor
		m_Descriptor.size = 16 + m_dwSize;
		fwrite(&m_Descriptor, sizeof(_WAVEDESCR), 1, file);

		// Save .WAV format
		fwrite(&m_Format, sizeof(_WAVEFORMAT), 1, file);

		// Write .WAV data
		BYTE id[4] = {'d', 'a', 't', 'a'};
		fwrite(id, sizeof(BYTE), 4, file);
		fwrite(&m_dwSize, sizeof(DWORD), 1, file);
		fwrite(m_lpData, sizeof(BYTE), m_dwSize, file);
		bResult = TRUE;

		// Close .WAV file
		fclose(file);
	}

	return bResult;
}

