#ifndef _CWAVE_H_
#define _CWAVE_H_

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef short SHORT;

#pragma pack(1)
typedef struct __WAVEDESCR
{
	BYTE riff[4];
	DWORD size;
	BYTE wave[4];

} _WAVEDESCR, *_LPWAVEDESCR;

typedef struct __WAVEFORMAT
{
	BYTE id[4];
	DWORD size;
	SHORT format;
	SHORT channels;
	DWORD sampleRate;
	DWORD byteRate;
	SHORT blockAlign;
	SHORT bitsPerSample;

} _WAVEFORMAT, *_LPWAVEFORMAT;
#pragma pack()


class CWave
{
public:
	CWave(void);
	virtual ~CWave(void);

	bool Save(wxString& filePath);

	_WAVEDESCR m_Descriptor;
	_WAVEFORMAT m_Format;
	LPBYTE m_lpData;
	DWORD m_dwSize;
	HWAVEOUT m_hWaveout;
	WAVEHDR m_WaveHeader;
};

#endif //_CWAVE_H_
