#ifndef __SINE_FUNCTION_HEADER__
#define __SINE_FUNCTION_HEADER__
#include <cmath>
#include <cassert>
#include <cstddef>
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
/*
		double result = m_amplitude * sin (m_currentPhase + m_phaseShift);
		if(result > 0.0)
			result = result + ((double)rand() * m_noiseSeed);
		else
			result = result - ((double)rand() * m_noiseSeed);
*/
		//double result = m_amplitude * sin (m_currentPhase + m_phaseShift);
		
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




WX_DECLARE_LIST(BaseFunctionGenerator, ListFunctionGenerator);

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ListFunctionGenerator);

class FunctionGenerator
{
	double m_sampleRate;
	portaudio::SampleDataFormat m_format;

	wxCriticalSection m_csDC;
	ListFunctionGenerator m_generators;


	template<typename T>
	void FillBufferInternalTemplate(void* buffer, unsigned int framesPerBuffer)
	{
		T *out = static_cast<T *>(buffer);
		for (unsigned int i = 0; i < 2 * framesPerBuffer; i += 2)
		{
			double val = 0.0;
			ListFunctionGenerator::iterator iter;
			for (iter = m_generators.begin(); iter != m_generators.end(); ++iter)
			{
				BaseFunctionGenerator *current = *iter;
				val += current->GenerateOneSample();
			}
			out[i]		= (T)val;
			out[i + 1]	= (T)val;
		}
	}
public:
	FunctionGenerator(portaudio::SampleDataFormat format, double sampleRate)
	{
		m_generators.DeleteContents(true);
		m_sampleRate = sampleRate;
		m_format = format;
	}

	~FunctionGenerator()
	{
		m_generators.Clear();
	}

	int AddSine(double amplitude, double freq, double phase)
	{
		wxCriticalSectionLocker locker(m_csDC);
		int index = m_generators.size();
		m_generators.Append(new SineFunctionGenerator(m_format, m_sampleRate, amplitude, freq, phase));
		return index;
	}

	void DelFunc(unsigned int index)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return;
		m_generators.DeleteObject(m_generators[index]);
	}


	int generate(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, 
		const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(m_generators.empty())
			return paContinue;

		ListFunctionGenerator::iterator iter;
		bool needClear = true;
		for (iter = m_generators.begin(); iter != m_generators.end(); ++iter)
		{
			BaseFunctionGenerator *current = *iter;
			int result = current->generate(outputBuffer, framesPerBuffer, needClear);
			needClear = false;
			if(result != paContinue)
				return result;
		}
		return paContinue;
	}

	void SetPhase(unsigned int index, double phase)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return;
		m_generators[index]->SetPhase(phase);
	}
	void SetFrequency(unsigned int index, double freq)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return;
		m_generators[index]->SetFreq(freq);
	}
	void SetAmplitude(unsigned int index, double amp)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return;
		m_generators[index]->SetAmplitude(amp);
	}

	void SetFormat(unsigned int index, portaudio::SampleDataFormat format)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return;
		m_generators[index]->SetFormat(format);
	}

	void SetSampleRate(unsigned int index, double sampleRate)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return;
		m_generators[index]->SetSampleRate(sampleRate);
	}

	const wxString FuncName(unsigned int index)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return _("Invalid device index");
		return m_generators[index]->FuncName();
	}
	void Reset()
	{
		wxCriticalSectionLocker locker(m_csDC);
		ListFunctionGenerator::iterator iter;
		//bool needClear = true;
		for (iter = m_generators.begin(); iter != m_generators.end(); ++iter)
		{
			BaseFunctionGenerator *current = *iter;
			current->Reset();
		}
	}

	void SetSampleRate(double sampleRate)
	{
		wxCriticalSectionLocker locker(m_csDC);
		m_sampleRate = sampleRate;
		ListFunctionGenerator::iterator iter;
		//bool needClear = true;
		for (iter = m_generators.begin(); iter != m_generators.end(); ++iter)
		{
			BaseFunctionGenerator *current = *iter;
			current->SetSampleRate(m_sampleRate);
		}
	}

	void SetFormat(portaudio::SampleDataFormat format)
	{
		wxCriticalSectionLocker locker(m_csDC);
		m_format = format;
		ListFunctionGenerator::iterator iter;
		//bool needClear = true;
		for (iter = m_generators.begin(); iter != m_generators.end(); ++iter)
		{
			BaseFunctionGenerator *current = *iter;
			current->SetFormat(m_format);
		}
	}

	double GetPhase(unsigned int index)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return 0;
		return m_generators[index]->GetPhase();
	}
	double GetFrequency(unsigned int index)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return 0;
		return m_generators[index]->GetFreq();
	}
	double GetAmplitude(unsigned int index)
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(index >= m_generators.size())
			return 0;
		return m_generators[index]->GetAmplitude();
	}

	double GetSampleRate()
	{
		wxCriticalSectionLocker locker(m_csDC);
		return m_sampleRate;
	}

	portaudio::SampleDataFormat GetFormat()
	{
		wxCriticalSectionLocker locker(m_csDC);
		return m_format;
	}

	void FillBuffer(void* buffer, unsigned int framesPerBuffer)
	{
		wxCriticalSectionLocker locker(m_csDC);

		ListFunctionGenerator::iterator iter;
		//bool needClear = true;
		for (iter = m_generators.begin(); iter != m_generators.end(); ++iter)
		{
			BaseFunctionGenerator *current = *iter;
			current->Reset();
		}
		
		switch(m_format)
		{
			case portaudio::INT16:
				FillBufferInternalTemplate<short>(buffer, framesPerBuffer);
				break;
			case portaudio::INT24:
				FillBufferInternalTemplate<Int24>(buffer, framesPerBuffer);
				break;
			case portaudio::INT32:
				FillBufferInternalTemplate<int>(buffer, framesPerBuffer);
				break;
			case portaudio::FLOAT32:
				FillBufferInternalTemplate<float>(buffer, framesPerBuffer);
				break;
		}
	}
};



class CaptureFunction
{
	SineGen *m_parentWindow;
public:
	CaptureFunction(SineGen *parentWindow, int tableSize, portaudio::SampleDataFormat format) : m_parentWindow(parentWindow), m_tableSize(tableSize), m_phase(0)
	{
		data_table.unknown_data = NULL;

		switch(format)
		{
			case portaudio::INT16:
				data_table.int16_table = new short[2 * tableSize];
				break;
			case portaudio::INT24:
				data_table.int24_table = new Int24[2 * tableSize];
				break;
			case portaudio::INT32:
				data_table.int32_table = new int[2 * tableSize];
				break;
			case portaudio::FLOAT32:
				data_table.float_table = new float[2 * tableSize];
				break;
		}
		m_format = format;
	}

	~CaptureFunction()
	{
		if(data_table.unknown_data)
		{
			switch(m_format)
			{
				case portaudio::INT16:
					delete[] data_table.int16_table;
					break;
				case portaudio::INT24:
					delete[] data_table.int24_table;
					break;
				case portaudio::INT32:
					delete[] data_table.int32_table;
					break;
				case portaudio::FLOAT32:
					delete[] data_table.float_table;
					break;
			}
		}
	}

	int capture(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, 
		const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
	{
		assert(inputBuffer != NULL);


		switch(m_format)
		{
			case portaudio::INT16:
				{
					short **in = (short **)(inputBuffer);

					for (unsigned int i = 0; i < framesPerBuffer; ++i)
					{
						data_table.int16_table[m_phase] = in[0][i];
						data_table.int16_table[m_phase + 1] = in[1][i];

						m_phase += 2;
						if (m_phase >= m_tableSize * 2)
						{
							m_phase = 0;
							m_parentWindow->UpdateData(data_table.int16_table, m_format, m_tableSize);
						}
					}
				}
				break;
			case portaudio::INT24:
				{
					Int24 **in = (Int24 **)(inputBuffer);

					for (unsigned int i = 0; i < framesPerBuffer; ++i)
					{
						data_table.int24_table[m_phase] = in[0][i];
						data_table.int24_table[m_phase + 1] = in[1][i];

						m_phase += 2;
						if (m_phase >= m_tableSize * 2)
						{
							m_phase = 0;
							m_parentWindow->UpdateData(data_table.int24_table, m_format, m_tableSize);
						}
					}
				}
				break;
			case portaudio::INT32:
				{
					int **in = (int **)(inputBuffer);

					for (unsigned int i = 0; i < framesPerBuffer; ++i)
					{
						data_table.int32_table[m_phase] = in[0][i];
						data_table.int32_table[m_phase + 1] = in[1][i];

						m_phase += 2;
						if (m_phase >= m_tableSize * 2)
						{
							m_phase = 0;
							m_parentWindow->UpdateData(data_table.int32_table, m_format, m_tableSize);
						}
					}
				}
				break;
			case portaudio::FLOAT32:
				{
					float **in = (float **)(inputBuffer);

					for (unsigned int i = 0; i < framesPerBuffer; ++i)
					{
						data_table.float_table[m_phase] = in[0][i];
						data_table.float_table[m_phase + 1] = in[1][i];

						m_phase += 2;
						if (m_phase >= m_tableSize * 2)
						{
							m_phase = 0;
							m_parentWindow->UpdateData(data_table.float_table, m_format, m_tableSize);
						}
					}
				}
				break;
		}
		return paContinue;
	}

private:
	portaudio::SampleDataFormat m_format;

	data_table_type data_table;

	int m_tableSize;
	
	int m_phase;
};
#endif // __SINE_FUNCTION_HEADER__