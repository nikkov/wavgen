#ifndef __SINE_FUNCTION_HEADER__
#define __SINE_FUNCTION_HEADER__
#include <cmath>
#include <cassert>
#include <cstddef>
#include "BaseFunction.h"

#define SEED_MAX_VAL	0.3

class SineFunctionGenerator : public BaseFunctionGenerator
{
	double m_freq;
	double m_phaseShift;
	double m_amplitude;

	double m_currentPhase;
	double m_currentPhaseDeg;
	double m_phaseStep;

	double m_noiseSeed;
protected:
	virtual void UpdateFormat(portaudio::SampleDataFormat format)
	{
		m_format = format;
		m_noiseSeed = SEED_MAX_VAL / GetMaxValueForFormat(format) / (double)RAND_MAX;
		UpdateAmplitude(m_amplitudeDb);
	}

	virtual void UpdateSampleRate(double samplerate)
	{
		m_sampleRate = samplerate;
		UpdateFreq(m_freq);
	}

	virtual void UpdatePhase(double phase)
	{
		m_currentPhaseDeg = phase;
		m_phaseShift = M_PI * m_currentPhaseDeg / 180.0;
	}
	virtual void UpdateFreq(double freq)
	{
		m_freq = freq;
		m_phaseStep = 2.0*M_PI*m_freq/m_sampleRate;
	}

	virtual void UpdateAmplitude(double amplitude)
	{
		m_amplitudeDb = amplitude;
		double maxValue = GetMaxValueForFormat(m_format);
		m_amplitude = maxValue * pow(10.0, m_amplitudeDb / 20.0);
	}

	virtual const wxString FuncNameInternal()
	{
		wxString name = wxString::Format("Sine F=%.3f; A=%.3f; Ph=%.3f", m_freq, m_amplitudeDb, m_currentPhaseDeg);
		return name;
	}


	virtual double GetPhaseInternal()
	{
		return m_currentPhaseDeg;
	}
	virtual double GetFreqInternal()
	{
		return m_freq;
	}
	virtual double GetAmplitudeInternal()
	{
		return m_amplitudeDb;
	}

	virtual void ResetInternal()
	{
		m_currentPhase = 0;
	}

	double calcNextVal()
	{
		double result = rand();
		result = (double)(m_noiseSeed * result + m_noiseSeed / 2.0);
		result = m_amplitude * (result + sin (m_currentPhase + m_phaseShift));
		m_currentPhase += m_phaseStep;
		if(m_currentPhase > 2.0*M_PI)
			m_currentPhase -= 2.0*M_PI;
		
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
	SineFunctionGenerator(portaudio::SampleDataFormat format, double sampleRate, double amplitude, double freq, double phase) : 
	  m_phaseShift(phase), m_currentPhase(0), m_currentPhaseDeg(0)
	{
		UpdateFormat(format);
		UpdateSampleRate(sampleRate);
		UpdateFreq(freq);
		UpdateAmplitude(amplitude);
	}
};

#endif // __SINE_FUNCTION_HEADER__