#ifndef __CONSTANT_FUNCTION_HEADER__
#define __CONSTANT_FUNCTION_HEADER__
#include "SineFunction.h"

class ConstantFunctionGenerator : public BaseFunctionGenerator
{
protected:
	double m_amplitudeDb;
	double m_sampleRate;
	portaudio::SampleDataFormat m_format;

	virtual void UpdateFormat(portaudio::SampleDataFormat format)
	{
		m_format = format;
	}

	virtual void UpdateSampleRate(double samplerate)
	{
		m_sampleRate = samplerate;
	}

	virtual void UpdatePhase(double phase)
	{
	}
	virtual void UpdateFreq(double freq)
	{
	}

	virtual void UpdateAmplitude(double amplitude)
	{
	}

	virtual const wxString FuncNameInternal()
	{
		wxString name = wxString::Format("Constant");
		return name;
	}


	virtual double GetPhaseInternal()
	{
		return 0;
	}
	virtual double GetFreqInternal()
	{
		return 0.0;
	}
	virtual double GetAmplitudeInternal()
	{
		return 1.0;
	}

	virtual void ResetInternal()
	{
	}

	double calcNextVal()
	{
		double result = 0.0;
		
		return result;
	}

	template<typename T>
	int generateInternalTemplate(void *outputBuffer, unsigned long framesPerBuffer, bool needClear)
	{
		T **out = static_cast<T **>(outputBuffer);
		if(needClear)
			for (unsigned int i = 0; i < framesPerBuffer; ++i)
			{
				double val = calcNextVal();
				out[0][i] = (T)val;
				out[1][i] = (T)val;

			}
		else
			for (unsigned int i = 0; i < framesPerBuffer; ++i)
			{
				double val = calcNextVal();
				out[0][i] += (T)val;
				out[1][i] += (T)val;
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
		return calcNextVal();
	}

public:
	ConstantFunctionGenerator(portaudio::SampleDataFormat format, double sampleRate)
	{
		UpdateFormat(format);
		UpdateSampleRate(sampleRate);
	}
};

#endif //__CONSTANT_FUNCTION_HEADER__
