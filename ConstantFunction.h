#ifndef __CONSTANT_FUNCTION_HEADER__
#define __CONSTANT_FUNCTION_HEADER__
#include "BaseFunction.h"

class ConstantFunctionGenerator : public BaseFunctionGenerator
{
protected:
	int m_amplitudeDb;

	virtual void UpdateFormat(portaudio::SampleDataFormat format)
	{
		m_format = format;
	}

	virtual void UpdateSampleRate(double samplerate) {}

	virtual void UpdatePhase(double phase) {}
	virtual void UpdateFreq(double freq) {}

	virtual void UpdateAmplitude(double amplitude) {}

	virtual const wxString FuncNameInternal()
	{
		wxString name = wxString::Format("Constant 0x%X", m_amplitudeDb);
		return name;
	}

	virtual double GetPhaseInternal() {	return 0; }
	virtual double GetFreqInternal() { return 0.0; }
	virtual double GetAmplitudeInternal() { return 1.0; }

	virtual void ResetInternal() {}

	template<typename T>
	int generateInternalTemplate(void *outputBuffer, unsigned long framesPerBuffer, bool needClear)
	{
		T **out = static_cast<T **>(outputBuffer);
		if (needClear)
			for (unsigned int i = 0; i < framesPerBuffer; ++i)
			{
				out[0][i] = (T)m_amplitudeDb;
				out[1][i] = (T)m_amplitudeDb;
			}
		else
			for (unsigned int i = 0; i < framesPerBuffer; ++i)
			{
				out[0][i] += (T)m_amplitudeDb;
				out[1][i] += (T)m_amplitudeDb;
			}
		return paContinue;
	}

	virtual int GenerateInternal(void *outputBuffer, unsigned long framesPerBuffer, bool needClear)
	{
		switch(m_format)
		{
			case portaudio::INT16:
				return generateInternalTemplate<short>(outputBuffer, framesPerBuffer, needClear);
			case portaudio::INT24:
				return generateInternalTemplate<Int24>(outputBuffer, framesPerBuffer, needClear);
			case portaudio::INT32:
				return generateInternalTemplate<int>(outputBuffer, framesPerBuffer, needClear);
			case portaudio::FLOAT32:
				return generateInternalTemplate<float>(outputBuffer, framesPerBuffer, needClear);
		}
		return paAbort;
	}

	virtual double GenerateOneSampleInternal()
	{
		return (double)m_amplitudeDb;
	}

public:
	ConstantFunctionGenerator(portaudio::SampleDataFormat format, int value) : m_amplitudeDb(value)
	{
		UpdateFormat(format);
	}
};

#endif //__CONSTANT_FUNCTION_HEADER__
