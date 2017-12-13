#ifndef __BASE_FUNCTION_HEADER__
#define __BASE_FUNCTION_HEADER__

#include "portaudiocpp/PortAudioCpp.hxx"

#include "sinegen.h"
#include "Int24.h"

union data_table_type
{
	void	*unknown_data;
	short	*int16_table;
	Int24	*int24_table;
	int		*int32_table;
	float	*float_table;
};


class BaseFunctionGenerator
{
	wxCriticalSection m_csDC;

protected:
	double m_amplitudeDb;
	double m_sampleRate;
	portaudio::SampleDataFormat m_format;

	virtual void UpdateFormat(portaudio::SampleDataFormat format) = 0;
	virtual void UpdatePhase(double phase) = 0;
	virtual void UpdateFreq(double freq) = 0;
	virtual void UpdateAmplitude(double amplitude) = 0;
	virtual void UpdateSampleRate(double samplerate) = 0;
	virtual const wxString FuncNameInternal() = 0;
	virtual int GenerateInternal(void *outputBuffer, unsigned long framesPerBuffer, bool needClear) = 0;
	virtual double GenerateOneSampleInternal() = 0;

	virtual double GetPhaseInternal() = 0;
	virtual double GetFreqInternal() = 0;
	virtual double GetAmplitudeInternal() = 0;

	virtual void ResetInternal() = 0;

public:
	BaseFunctionGenerator() : m_amplitudeDb(0), m_sampleRate(0)
	{
	}
	virtual ~BaseFunctionGenerator()
	{
	}
	int generate(void *outputBuffer, unsigned long framesPerBuffer, bool needClear)
	{
		wxCriticalSectionLocker locker(m_csDC);
		assert(outputBuffer != NULL);
		return GenerateInternal(outputBuffer, framesPerBuffer, needClear);
	}
	double GenerateOneSample()
	{
		return 	GenerateOneSampleInternal();
	}

	void SetFreq(double freq)
	{
		wxCriticalSectionLocker locker(m_csDC);
		UpdateFreq(freq);
	}
	void SetAmplitude(double amp)
	{
		wxCriticalSectionLocker locker(m_csDC);
		UpdateAmplitude(amp);
	}
	void SetPhase(double phase)
	{
		wxCriticalSectionLocker locker(m_csDC);
		UpdatePhase(phase);
	}
	void SetSampleRate(double samplerate)
	{
		wxCriticalSectionLocker locker(m_csDC);
		UpdateSampleRate(samplerate);
	}
	void SetFormat(portaudio::SampleDataFormat format)
	{
		wxCriticalSectionLocker locker(m_csDC);
		UpdateFormat(format);
	}
	const wxString FuncName()
	{
		wxCriticalSectionLocker locker(m_csDC);
		return FuncNameInternal();
	}

	double GetFreq()
	{
		wxCriticalSectionLocker locker(m_csDC);
		return GetFreqInternal();
	}
	double GetAmplitude()
	{
		wxCriticalSectionLocker locker(m_csDC);
		return GetAmplitudeInternal();
	}
	double GetPhase()
	{
		wxCriticalSectionLocker locker(m_csDC);
		return GetPhaseInternal();
	}
	void Reset()
	{
		wxCriticalSectionLocker locker(m_csDC);
		ResetInternal();
	}

};

#endif //__BASE_FUNCTION_HEADER__
