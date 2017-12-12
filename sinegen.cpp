#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/imaglist.h"
#include <wx/graphics.h>
#include <wx/spinctrl.h>

#include "sinegen.h"
#include "sinefunction.h"
#include "constantfunction.h"
#include "wave/wave.h"

static double STANDARD_SAMPLE_RATES[] = {
		8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
		44100.0, 48000.0, 88200.0, 96000.0, 192000.0, 384000.0, -1 }; // negative terminated list

double GetMaxValueForFormat(portaudio::SampleDataFormat format)
{
	switch(format)
	{
		case portaudio::INT16:
			return 32500.0;
			break;
		case portaudio::INT24:
			return 8320000.0;
			break;
		case portaudio::INT32:
			return 2129920000.0;
			break;
	}
	return 1.0f;
}

int GetSizeForFormat(portaudio::SampleDataFormat format)
{
	switch(format)
	{
		case portaudio::INT16:
			return 2;
			break;
		case portaudio::INT24:
			return 3;
			break;
		case portaudio::INT32:
			return 4;
			break;
	}
	return 0;
}


IMPLEMENT_DYNAMIC_CLASS( SineGen, wxDialog )


BEGIN_EVENT_TABLE( SineGen, wxDialog )
    EVT_BUTTON( XRCID("ID_BUTTON_OUTPUT_START"), SineGen::OnButtonOutputStartClick )
    EVT_UPDATE_UI( XRCID("ID_BUTTON_OUTPUT_START"), SineGen::OnButtonOutputStartUpdate )
    EVT_BUTTON( XRCID("ID_BUTTON_OUTPUT_STOP"), SineGen::OnButtonOutputStopClick )
    EVT_UPDATE_UI( XRCID("ID_BUTTON_OUTPUT_STOP"), SineGen::OnButtonOutputStopUpdate )

    EVT_BUTTON( XRCID("ID_BUTTON_INPUT_START"), SineGen::OnButtonInputStartClick )
    EVT_UPDATE_UI( XRCID("ID_BUTTON_INPUT_START"), SineGen::OnButtonInputStartUpdate )
    EVT_BUTTON( XRCID("ID_BUTTON_INPUT_STOP"), SineGen::OnButtonInputStopClick )
    EVT_UPDATE_UI( XRCID("ID_BUTTON_INPUT_STOP"), SineGen::OnButtonInputStopUpdate )

	EVT_CHECKBOX(XRCID("ID_CHECKBOX_WAVEFORM"), SineGen::OnWaveformCheckboxClicked)

    EVT_BUTTON( XRCID("ID_BUTTON_SAVE_WAV"), SineGen::OnButtonSaveWavClick )
    EVT_UPDATE_UI( XRCID("ID_BUTTON_SAVE_WAV"), SineGen::OnButtonOutputStartUpdate )

	EVT_LISTBOX(XRCID("ID_LIST_COMPONENTS"), SineGen::OnSineListBoxSelectChanged)

    EVT_BUTTON( XRCID("ID_ADD_SINE_BUTTON"), SineGen::OnButtonAddSineClick )
    EVT_UPDATE_UI( XRCID("ID_ADD_SINE_BUTTON"), SineGen::OnButtonOutputStartUpdate )
    EVT_BUTTON( XRCID("ID_DEL_SINE_BUTTON"), SineGen::OnButtonDelSineClick )
    EVT_UPDATE_UI( XRCID("ID_DEL_SINE_BUTTON"), SineGen::OnButtonOutputStartUpdate )

	EVT_COMMAND_SCROLL( XRCID("ID_AMPLITUDE_SLIDER"), SineGen::OnSliderAmpScrollCmd )
	EVT_COMMAND_SCROLL( XRCID("ID_FREQUENCY_SLIDER"), SineGen::OnSliderFreqScrollCmd )
	EVT_COMMAND_SCROLL( XRCID("ID_PHASE_SLIDER"), SineGen::OnSliderPhaseScrollCmd )

	EVT_TEXT_ENTER( XRCID("ID_CUR_AMP_VALUE"), SineGen::OnAmplitudeTextEnter )
	EVT_TEXT_ENTER( XRCID("ID_CUR_FREQ_VALUE"), SineGen::OnFrequencyTextEnter )
	EVT_TEXT_ENTER( XRCID("ID_CUR_PHASE_VALUE"), SineGen::OnPhaseTextEnter )

	EVT_UPDATE_UI( XRCID("ID_AMPLITUDE_SLIDER"), SineGen::OnCurrentValueUpdate )
	EVT_UPDATE_UI( XRCID("ID_FREQUENCY_SLIDER"), SineGen::OnCurrentValueUpdate )
	EVT_UPDATE_UI( XRCID("ID_PHASE_SLIDER"), SineGen::OnCurrentValueUpdate )
	EVT_UPDATE_UI( XRCID("ID_CUR_AMP_VALUE"), SineGen::OnCurrentValueUpdate )
	EVT_UPDATE_UI( XRCID("ID_CUR_FREQ_VALUE"), SineGen::OnCurrentValueUpdate )
	EVT_UPDATE_UI( XRCID("ID_CUR_PHASE_VALUE"), SineGen::OnCurrentValueUpdate )

    EVT_COMBOBOX( XRCID("ID_CHOICE_OUT_DEVICE"), SineGen::OnChoiceOutDeviceSelected )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_OUT_DEVICE"), SineGen::OnChoiceOutUpdate )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_OUT_FREQ"), SineGen::OnChoiceOutUpdate )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_OUT_DEPTH"), SineGen::OnChoiceOutUpdate )

    EVT_COMBOBOX( XRCID("ID_CHOICE_IN_DEVICE"), SineGen::OnChoiceInDeviceSelected )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_IN_DEVICE"), SineGen::OnChoiceInUpdate )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_IN_FREQ"), SineGen::OnChoiceInUpdate )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_IN_DEPTH"), SineGen::OnChoiceInUpdate )

    EVT_UPDATE_UI( XRCID("ID_CHOICE_FFT_SIZE"), SineGen::OnChoiceInUpdate )
    EVT_UPDATE_UI( XRCID("ID_CHOICE_WND_FUNC"), SineGen::OnChoiceInUpdate )

	EVT_SPINCTRL( XRCID("ID_MAX_VALUE_VIEW"), SineGen::OnSpinCtrlMaxScaleChange )
	EVT_SPINCTRL( XRCID("ID_MIN_VALUE_VIEW"), SineGen::OnSpinCtrlMinScaleChange )

END_EVENT_TABLE()


 SineGen::SineGen()
{
    Init();
}

SineGen::SineGen( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


bool SineGen::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}


SineGen::~SineGen()
{
	StopInput();
	StopOutput();

	if(m_PaudioCl)
		delete m_PaudioCl;
}


void SineGen::Init()
{
	m_valueChangedFromCode = false;
	m_PaudioCl = NULL;
	m_outputStream = NULL;
	m_functionGenerator = NULL;
	m_inputStream = NULL;
	m_captureFunction = NULL;

	m_PaudioCl = new PaudioCl();

	m_functionGenerator = new FunctionGenerator(portaudio::INT16, 44100.0);
}


void SineGen::CreateControls()
{    
	try
	{
		if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_SINEGEN")))
			wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()"));

		m_signalPanel = new SignalView(this, XRCID("ID_PANEL_SIGNAL"), wxDefaultPosition, wxSize(300, 400), wxNO_BORDER);
		if (m_signalPanel)
			wxXmlResource::Get()->AttachUnknownControl(wxT("ID_PANEL_SIGNAL"), m_signalPanel);

		m_choiceOutputDevice = XRCCTRL(*this, "ID_CHOICE_OUT_DEVICE", wxChoice);
		m_choiceOutputFreq = XRCCTRL(*this, "ID_CHOICE_OUT_FREQ", wxChoice);
		m_choiceOutputDepth = XRCCTRL(*this, "ID_CHOICE_OUT_DEPTH", wxChoice);

		m_choiceInputDevice = XRCCTRL(*this, "ID_CHOICE_IN_DEVICE", wxChoice);
		m_choiceInputFreq = XRCCTRL(*this, "ID_CHOICE_IN_FREQ", wxChoice);
		m_choiceInputDepth = XRCCTRL(*this, "ID_CHOICE_IN_DEPTH", wxChoice);

		m_choiceInputFFTSize = XRCCTRL(*this, "ID_CHOICE_FFT_SIZE", wxChoice);
		m_choiceInputWndFunc = XRCCTRL(*this, "ID_CHOICE_WND_FUNC", wxChoice);

		m_checkBoxWaveform = XRCCTRL(*this, "ID_CHECKBOX_WAVEFORM", wxCheckBox);

		m_componentsListBox = XRCCTRL(*this, "ID_LIST_COMPONENTS", wxListBox);

		m_newSineFreq = XRCCTRL(*this, "ID_NEW_SINE_FREQ", wxTextCtrl);
		m_newSineAmpl = XRCCTRL(*this, "ID_NEW_SINE_AMPLITUDE", wxTextCtrl);
		m_newSinePhase = XRCCTRL(*this, "ID_NEW_SINE_PHASE", wxTextCtrl);

		m_frequencySlider = XRCCTRL(*this, "ID_FREQUENCY_SLIDER", wxSlider);
		m_amplitudeSlider = XRCCTRL(*this, "ID_AMPLITUDE_SLIDER", wxSlider);
		m_phaseSlider = XRCCTRL(*this, "ID_PHASE_SLIDER", wxSlider);

		m_curSineFreq = XRCCTRL(*this, "ID_CUR_FREQ_VALUE", wxTextCtrl);
		m_curSineAmpl = XRCCTRL(*this, "ID_CUR_AMP_VALUE", wxTextCtrl);
		m_curSinePhase = XRCCTRL(*this, "ID_CUR_PHASE_VALUE", wxTextCtrl);

		m_scaleMaxValue = XRCCTRL(*this, "ID_MAX_VALUE_VIEW", wxSpinCtrl);
		m_scaleMinValue = XRCCTRL(*this, "ID_MIN_VALUE_VIEW", wxSpinCtrl);
	}
	catch(...)
	{
		wxLogError(wxT("Error initialize controls in OnInit()"));
	}

	FillInOutDevices();
}


bool SineGen::ShowToolTips()
{
    return true;
}

wxBitmap SineGen::GetBitmapResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon SineGen::GetIconResource( const wxString& name )
{
    wxUnusedVar(name);
    return wxNullIcon;
}


void SineGen::OnChoiceOutDeviceSelected( wxCommandEvent& event )
{
	FillSampleFreqAndDepth(false);
}


void SineGen::OnChoiceInDeviceSelected( wxCommandEvent& event )
{
	FillSampleFreqAndDepth(true);
}

void SineGen::OnWaveformCheckboxClicked( wxCommandEvent& event )
{
	m_signalPanel->ShowWaveform(m_checkBoxWaveform->IsChecked());
}

void SineGen::OnSliderFreqScrollCmd( wxScrollEvent& event )
{
	double value = m_frequencySlider->GetValue();
	if(!m_valueChangedFromCode)
		UpdateCurrentGeneratorValue(m_curSineFreq, value);
}


void SineGen::OnSliderAmpScrollCmd( wxScrollEvent& event )
{
	double value = m_amplitudeSlider->GetValue();
	if(!m_valueChangedFromCode)
		UpdateCurrentGeneratorValue(m_curSineAmpl, value);
}

void SineGen::OnSliderPhaseScrollCmd( wxScrollEvent& event )
{
	double value = m_phaseSlider->GetValue();
	if(!m_valueChangedFromCode)
		UpdateCurrentGeneratorValue(m_curSinePhase, value);
}


void SineGen::FillInOutDevices()
{
	if(!m_choiceOutputDevice || !m_choiceInputDevice)
		return;

	try
	{
		m_choiceOutputDevice->Clear();
		int outDeviceNum = m_PaudioCl->GetDevicesNum(false);
		int defIndex = 0;
		for(int i = 0; i < outDeviceNum; i++)
		{
			portaudio::Device& device = m_PaudioCl->GetDevice(false, i);

			wxString u8name = wxString::Format(u8"[%s]%s", device.hostApi().name(), device.name());
			wxString name((const char*)u8name.c_str(), wxConvUTF8);

			m_choiceOutputDevice->Append(name);
			if(device.isSystemDefaultOutputDevice())
				defIndex = i;
		}
		if(outDeviceNum > 0)
			m_choiceOutputDevice->Select(defIndex);

		FillSampleFreqAndDepth(false);
	}
	catch (const portaudio::PaException &e)
	{
		wxMessageBox(wxString::Format(wxT("Error fill output devices parameters, a PortAudio error occured: %s"), e.paErrorText()), "Error!");
	}
	catch (const portaudio::PaCppException &e)
	{
		wxMessageBox(wxString::Format(wxT("Error fill output devices parameters, a PortAudioCpp error occured: %s"), e.what()), "Error!");
	}
	catch (const std::exception &e)
	{
		wxMessageBox(wxString::Format(wxT("Error fill output devices parameters, a generic exception occured: %s"), e.what()), "Error!");
	}
	catch(...)
	{
		wxLogError(wxT("Error fill output devices parameters, unknown error"));
	}

	try
	{
		m_choiceInputDevice->Clear();
		int inDeviceNum = m_PaudioCl->GetDevicesNum(true);
		int defIndex = 0;
		for(int i = 0; i < inDeviceNum; i++)
		{
			portaudio::Device& device = m_PaudioCl->GetDevice(true, i);
			m_choiceInputDevice->Append(wxString::Format("[%s]%s", device.hostApi().name(), device.name()));
			if(device.isSystemDefaultInputDevice())
				defIndex = i;
		}
		if(inDeviceNum > 0)
			m_choiceInputDevice->Select(defIndex);
		
		FillSampleFreqAndDepth(true);
	}
	catch (const portaudio::PaException &e)
	{
		wxMessageBox(wxString::Format(wxT("Error fill input devices parameters, a PortAudio error occured: %s"), e.paErrorText()), "Error!");
	}
	catch (const portaudio::PaCppException &e)
	{
		wxMessageBox(wxString::Format(wxT("Error fill input devices parameters, a PortAudioCpp error occured: %s"), e.what()), "Error!");
	}
	catch (const std::exception &e)
	{
		wxMessageBox(wxString::Format(wxT("Error fill input devices parameters, a generic exception occured: %s"), e.what()), "Error!");
	}
	catch(...)
	{
		wxLogError(wxT("Error fill input devices parameters, unknown error"));
	}
}

void SineGen::FillSampleFreqAndDepth(bool isInput)
{
	wxChoice *comboDevice = NULL;
	wxChoice *comboFreq = NULL;
	wxChoice *comboDepth = NULL;
	if(isInput)
	{
		comboDevice = m_choiceInputDevice;
		comboFreq = m_choiceInputFreq;
		comboDepth = m_choiceInputDepth;
	}
	else
	{
		comboDevice = m_choiceOutputDevice;
		comboFreq = m_choiceOutputFreq;
		comboDepth = m_choiceOutputDepth;
	}
	comboFreq->Clear();
	comboDepth->Select(0);

	int devIndex = comboDevice->GetCurrentSelection();
	if(devIndex == -1)
		devIndex = 0;
	portaudio::Device& device = m_PaudioCl->GetDevice(isInput, devIndex);

	portaudio::DirectionSpecificStreamParameters inputParameters(device, device.maxInputChannels(), portaudio::INT16, true, 0.0, NULL);
	portaudio::DirectionSpecificStreamParameters outputParameters(device, device.maxOutputChannels(), portaudio::INT16, true, 0.0, NULL);

	int defaultFreqIndex = -1;
	int supportedCount = -1;
	for (int i = 0; STANDARD_SAMPLE_RATES[i] > 0; ++i)
	{
		portaudio::StreamParameters tmp = portaudio::StreamParameters(inputParameters, outputParameters, STANDARD_SAMPLE_RATES[i], 0, paNoFlag);

		if (tmp.isSupported())
		{
			supportedCount++;
			comboFreq->Append(wxString::Format("%d", (int)STANDARD_SAMPLE_RATES[i]));
			if(defaultFreqIndex == -1 && (STANDARD_SAMPLE_RATES[i] == 44100.0 || STANDARD_SAMPLE_RATES[i] == 48000.0))
			{
				defaultFreqIndex = supportedCount;
			}
		}
	}
	if(defaultFreqIndex >= 0)
		comboFreq->Select(defaultFreqIndex);
}


bool SineGen::CreateOutputStream()
{
	try
	{
		int devIndex = m_choiceOutputDevice->GetCurrentSelection();
		portaudio::Device& device = m_PaudioCl->GetDevice(false, devIndex);
		portaudio::SampleDataFormat out_format = portaudio::INT16;

		switch(m_choiceOutputDepth->GetCurrentSelection())
		{
			case 0:
				out_format = portaudio::INT16;
				break;
			case 1:
				out_format = portaudio::INT24;
				break;
			case 2:
				out_format = portaudio::INT32;
				break;
		}

		long sample_freq = 44100;
		m_choiceOutputFreq->GetString(m_choiceOutputFreq->GetCurrentSelection()).ToLong(&sample_freq);
		int frames_per_buffer = 64;

		portaudio::DirectionSpecificStreamParameters outParams(device, 2, out_format, false, device.defaultLowOutputLatency(), NULL);
		portaudio::StreamParameters params(portaudio::DirectionSpecificStreamParameters::null(), outParams, sample_freq, frames_per_buffer, paClipOff);
		m_functionGenerator->SetSampleRate(sample_freq);
		m_functionGenerator->SetFormat(out_format);

		// Create (and open) a new Stream, using the SineGenerator::generate function as a callback:
		m_outputStream = new portaudio::MemFunCallbackStream<FunctionGenerator>(params, *m_functionGenerator, &FunctionGenerator::generate);

		// Start the Stream (audio playback starts):
		m_outputStream->start();
		return true;
	}
	catch (const portaudio::PaException &e)
	{
		wxMessageBox(wxString::Format("A PortAudio error occured: %s", e.paErrorText()), "Error!");
	}
	catch (const portaudio::PaCppException &e)
	{
		wxMessageBox(wxString::Format("A PortAudioCpp error occured: %s", e.what()), "Error!");
	}
	catch (const std::exception &e)
	{
		wxMessageBox(wxString::Format("A generic exception occured: %s", e.what()), "Error!");
	}
	catch (...)
	{
		wxMessageBox("An unknown exception occured.", "Error!");
	}
	if(m_outputStream)
		delete m_outputStream;
	m_outputStream = NULL;

	return false;
}

bool SineGen::CreateInputStream()
{
	try
	{
		int devIndex = m_choiceInputDevice->GetCurrentSelection();
		portaudio::Device& device = m_PaudioCl->GetDevice(true, devIndex);
		portaudio::SampleDataFormat in_format = portaudio::INT16;

		switch(m_choiceInputDepth->GetCurrentSelection())
		{
			case 0:
				in_format = portaudio::INT16;
				break;
			case 1:
				in_format = portaudio::INT24;
				break;
			case 2:
				in_format = portaudio::INT32;
				break;
		}

		long sample_freq = 44100;
		m_choiceInputFreq->GetString(m_choiceInputFreq->GetCurrentSelection()).ToLong(&sample_freq);
		int frames_per_buffer = 64;

		portaudio::DirectionSpecificStreamParameters inParams(device, 2, in_format, false, device.defaultLowOutputLatency(), NULL);
		portaudio::StreamParameters params(inParams, portaudio::DirectionSpecificStreamParameters::null(), sample_freq, frames_per_buffer, paClipOff);

		long buffer_size = 16384;
		m_choiceInputFFTSize->GetString(m_choiceInputFFTSize->GetCurrentSelection()).ToLong(&buffer_size);

		m_captureFunction = new CaptureFunction(this, buffer_size, in_format);
		m_signalPanel->SetSignalParams(buffer_size, (float)sample_freq, (namespace_utilfunc::WindowFuncType)m_choiceInputWndFunc->GetCurrentSelection());

		// Create (and open) a new Stream, using the SineGenerator::generate function as a callback:
		m_inputStream = new portaudio::MemFunCallbackStream<CaptureFunction>(params, *m_captureFunction, &CaptureFunction::capture);

		// Start the Stream (audio playback starts):
		m_inputStream->start();
		return true;
	}
	catch (const portaudio::PaException &e)
	{
		wxMessageBox(wxString::Format("A PortAudio error occured: %s", e.paErrorText()), "Error!");
	}
	catch (const portaudio::PaCppException &e)
	{
		wxMessageBox(wxString::Format("A PortAudioCpp error occured: %s", e.what()), "Error!");
	}
	catch (const std::exception &e)
	{
		wxMessageBox(wxString::Format("A generic exception occured: %s", e.what()), "Error!");
	}
	catch (...)
	{
		wxMessageBox("An unknown exception occured.", "Error!");
	}
	if(m_inputStream)
		delete m_inputStream;
	m_inputStream = NULL;

	if(m_captureFunction)
		delete m_captureFunction;
	m_captureFunction = NULL;

	return false;
}

bool SineGen::DestroyOutputStream()
{
	if(m_outputStream)
	{
		m_outputStream->stop();
		m_outputStream->close();
		delete m_outputStream;
		m_outputStream = NULL;

		m_functionGenerator->Reset();

		return true;
	}
	else
		return false;
}

bool SineGen::DestroyInputStream()
{
	if(m_inputStream)
	{
		m_inputStream->stop();
		m_inputStream->close();
		delete m_inputStream;
		m_inputStream = NULL;
		delete m_captureFunction;
		m_captureFunction = NULL;
		return true;
	}
	else
		return false;
}


bool SineGen::StartOutput()
{
	if(!CreateOutputStream())
		return false;
	return true;
}

bool SineGen::StartInput()
{
	if(!CreateInputStream())
		return false;
	return true;
}

bool SineGen::StopOutput()
{
	bool result = DestroyOutputStream();
	//m_signalPanel->ClearDisplay();
	return result;
}

bool SineGen::StopInput()
{
	bool result = DestroyInputStream();
	//m_signalPanel->ClearDisplay();
	return result;
}

void SineGen::OnChoiceOutUpdate( wxUpdateUIEvent& event )
{
	event.Enable(m_outputStream == NULL);
}

void SineGen::OnChoiceInUpdate( wxUpdateUIEvent& event )
{
	event.Enable(m_inputStream == NULL );
}

void SineGen::UpdateData(void* data, portaudio::SampleDataFormat format, int samples)
{
	m_signalPanel->UpdateData(data, format, samples);
}


void SineGen::OnButtonOutputStartClick( wxCommandEvent& event )
{
	StartOutput();

}

void SineGen::OnButtonOutputStopClick( wxCommandEvent& event )
{
	StopOutput();
}

void SineGen::OnButtonOutputStartUpdate( wxUpdateUIEvent& event )
{
	event.Enable(m_outputStream == NULL);
}

void SineGen::OnButtonOutputStopUpdate( wxUpdateUIEvent& event )
{
	event.Enable(m_outputStream != NULL);
}

void SineGen::OnButtonInputStartClick( wxCommandEvent& event )
{
	StartInput();

}

void SineGen::OnButtonInputStopClick( wxCommandEvent& event )
{
	StopInput();
}

void SineGen::OnButtonInputStartUpdate( wxUpdateUIEvent& event )
{
	event.Enable(m_inputStream == NULL);
}

void SineGen::OnButtonInputStopUpdate( wxUpdateUIEvent& event )
{
	event.Enable(m_inputStream != NULL);
}

void SineGen::OnButtonAddSineClick( wxCommandEvent& event )
{
	double freqVal = 1000;
	double ampVal = -6;
	double phaseVal = 0;
	long sample_freq = 44100;
	portaudio::SampleDataFormat out_format = portaudio::INT16;

	m_choiceOutputFreq->GetString(m_choiceOutputFreq->GetCurrentSelection()).ToLong(&sample_freq);

	switch(m_choiceOutputDepth->GetCurrentSelection())
	{
		case 0:
			out_format = portaudio::INT16;
			break;
		case 1:
			out_format = portaudio::INT24;
			break;
		case 2:
			out_format = portaudio::INT32;
			break;
	}

	m_newSineFreq->GetValue().ToCDouble(&freqVal);
	m_newSineAmpl->GetValue().ToCDouble(&ampVal);
	m_newSinePhase->GetValue().ToCDouble(&phaseVal);

	int index = m_functionGenerator->AddSine(ampVal, freqVal, phaseVal);
	m_functionGenerator->SetFormat(index, out_format);
	m_functionGenerator->SetSampleRate(index, sample_freq);
	wxString newFunc = m_functionGenerator->FuncName(index);

	m_componentsListBox->Append(newFunc);
	m_componentsListBox->SetSelection(index);
	UpdateCurrentGeneratorValues();
}

void SineGen::OnButtonDelSineClick( wxCommandEvent& event )
{
	int index = m_componentsListBox->GetSelection();
	if(index == -1)
		return;
	m_functionGenerator->DelFunc(index);
	m_componentsListBox->Delete(index);
	if(m_componentsListBox->GetCount() > 0)
	{
		m_componentsListBox->SetSelection(0);
		UpdateCurrentGeneratorValues();
	}
	else
		m_componentsListBox->SetSelection(-1);
}

void SineGen::OnSineListBoxSelectChanged( wxCommandEvent& event )
{
	int index = m_componentsListBox->GetSelection();
	if(index == -1)
		return;
	UpdateCurrentGeneratorValues();
}

void SineGen::UpdateCurrentGeneratorValues()
{
	int index = m_componentsListBox->GetSelection();
	if(index != -1)
	{
		m_valueChangedFromCode = true;
		double freqVal = m_functionGenerator->GetFrequency(index);
		double ampVal = m_functionGenerator->GetAmplitude(index);
		double phaseVal = m_functionGenerator->GetPhase(index);

		m_curSineAmpl->SetValue(wxString::Format("%.3f", ampVal));
		m_curSineFreq->SetValue(wxString::Format("%.3f", freqVal));
		m_curSinePhase->SetValue(wxString::Format("%.3f", phaseVal));

		m_amplitudeSlider->SetValue((int)ampVal);
		m_frequencySlider->SetValue((int)freqVal);
		m_phaseSlider->SetValue((int)phaseVal);
		m_componentsListBox->SetString(index, m_functionGenerator->FuncName(index));
		m_valueChangedFromCode = false;
	}
}


void SineGen::UpdateCurrentGeneratorValue(wxTextCtrl* textCtrl, double value)
{
	textCtrl->SetValue(wxString::Format("%.3f", value));
	int index = m_componentsListBox->GetSelection();
	if(index != -1)
	{
		if(textCtrl == m_curSineAmpl)
			m_functionGenerator->SetAmplitude(index, value);
		if(textCtrl == m_curSineFreq)
			m_functionGenerator->SetFrequency(index, value);
		if(textCtrl == m_curSinePhase)
			m_functionGenerator->SetPhase(index, value);

		m_componentsListBox->SetString(index, m_functionGenerator->FuncName(index));
	}
}


void SineGen::UpdateSliderValue(wxTextCtrl* textCtrl, wxSlider *sliderCtrl)
{
	if(m_valueChangedFromCode)
		return;

	double value = 0;
	if(textCtrl->GetValue().ToCDouble(&value))
	{
		m_valueChangedFromCode = true;

		if(sliderCtrl->GetMax() < value)
			value = sliderCtrl->GetMax();
		if(sliderCtrl->GetMin() > value)
			value = sliderCtrl->GetMin();
		sliderCtrl->SetValue((int)value);
		UpdateCurrentGeneratorValue(textCtrl, value);

		m_valueChangedFromCode = false;
	}
}


void SineGen::OnAmplitudeTextEnter( wxCommandEvent& )
{
	UpdateSliderValue(m_curSineAmpl, m_amplitudeSlider);
}

void SineGen::OnFrequencyTextEnter( wxCommandEvent& )
{
	UpdateSliderValue(m_curSineFreq, m_frequencySlider);
}

void SineGen::OnPhaseTextEnter( wxCommandEvent& )
{
	UpdateSliderValue(m_curSinePhase, m_phaseSlider);
}

void SineGen::OnCurrentValueUpdate( wxUpdateUIEvent& event )
{
	int index = m_componentsListBox->GetSelection();
	event.Enable(index != -1);
}

void SineGen::OnSpinCtrlMaxScaleChange( wxSpinEvent& event )
{
	int value = m_scaleMaxValue->GetValue();
	m_signalPanel->SetYMaxScale(value);
}

void SineGen::OnSpinCtrlMinScaleChange( wxSpinEvent& event )
{
	int value = m_scaleMinValue->GetValue();
	m_signalPanel->SetYMinScale(value);
}


void SineGen::OnButtonSaveWavClick( wxCommandEvent& event )
{
	wxFileDialog openDlg(this, "Open file", wxEmptyString, wxEmptyString, "Log files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if(openDlg.ShowModal() == wxID_OK)
	{
		wxString pathFile = openDlg.GetPath();
		wxString pathDir = openDlg.GetDirectory();

		FILE* inp = fopen(pathFile, "rt");
		char tbuff[128];
		BYTE* lpData = new BYTE[1024*1024];
		float tms;
		int ch, val, pos;
		if(inp)
		{
			int cnt = 0;
			pos = 0;
			fgets(tbuff, sizeof(tbuff), inp);
			do
			{
				if(fgets(tbuff, sizeof(tbuff), inp))
				{
					sscanf(tbuff, "%f,%d,%x", &tms, &ch, &val);
					val = val >> 8;
					memcpy(lpData + cnt * 2, ((BYTE*)&val), 2);
					cnt++;
				}
			}
			while(!feof(inp));
		
			CWave* wav = new CWave();
			int bits = 16;

			wav->m_Format.sampleRate = 44100;
			wav->m_Format.channels = 2;
			wav->m_Format.byteRate = wav->m_Format.sampleRate * wav->m_Format.channels * bits / 8;
			wav->m_Format.blockAlign = wav->m_Format.channels * bits / 8;
			wav->m_Format.bitsPerSample = bits;

			wav->m_dwSize = cnt * bits / 8;
			wav->m_lpData = (LPBYTE)new BYTE[wav->m_dwSize];
			memcpy(wav->m_lpData, lpData, wav->m_dwSize);
			pathDir += "\\out.wav";
			wav->Save(pathDir);

			delete wav->m_lpData;
			delete wav;


			fclose(inp);
		}
		delete lpData;
		return;
	}


	wxFileDialog dlg(this, "Save file as...", wxEmptyString, wxEmptyString, "WAV files (*.wav)|*.wav", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	portaudio::SampleDataFormat out_format = portaudio::INT16;

	switch(m_choiceOutputDepth->GetCurrentSelection())
	{
		case 0:
			out_format = portaudio::INT16;
			break;
		case 1:
			out_format = portaudio::INT24;
			break;
		case 2:
			out_format = portaudio::INT32;
			break;
	}

	long sample_freq = 44100;
	m_choiceOutputFreq->GetString(m_choiceOutputFreq->GetCurrentSelection()).ToLong(&sample_freq);
	
	m_functionGenerator->SetSampleRate(sample_freq);
	m_functionGenerator->SetFormat(out_format);

	if(dlg.ShowModal() == wxID_OK)
	{
		int durationFile = 30;
		CWave* wav = new CWave();

		wav->m_Format.sampleRate = m_functionGenerator->GetSampleRate();																// Sample Rate
		wav->m_Format.byteRate = wav->m_Format.sampleRate * wav->m_Format.channels * GetSizeForFormat(m_functionGenerator->GetFormat());// Byterate
		wav->m_Format.blockAlign = wav->m_Format.channels * GetSizeForFormat(m_functionGenerator->GetFormat());							// Frame size
		wav->m_Format.bitsPerSample = 8 * GetSizeForFormat(m_functionGenerator->GetFormat());											// Bits per sample

		wav->m_dwSize = durationFile * wav->m_Format.byteRate;
		wav->m_lpData = (LPBYTE)new BYTE[wav->m_dwSize];
#if 0
		if(out_format == portaudio::INT24)
			for(int i = 0; i < wav->m_dwSize; i += 6)
			{
				wav->m_lpData[i]   = 0xAA; //10101010
				wav->m_lpData[i+1] = 0x92; //10010010
				wav->m_lpData[i+2] = 0x88; //10001000
				wav->m_lpData[i+3] = 0x55; //01010101
				wav->m_lpData[i+4] = 0x49; //01001001
				wav->m_lpData[i+5] = 0x11; //00010001
			}
		else
			for(int i = 0; i < wav->m_dwSize; i += 4)
			{
				wav->m_lpData[i]   = 0xAA; //10101010
				wav->m_lpData[i+1] = 0x92; //10010010
				wav->m_lpData[i+2] = 0x55; //01010101
				wav->m_lpData[i+3] = 0x49; //01001001
			}
#else
		m_functionGenerator->FillBuffer(wav->m_lpData, wav->m_dwSize / wav->m_Format.blockAlign);
#endif

		wxString pathFile = dlg.GetPath();
		wav->Save(pathFile);

		delete wav->m_lpData;
		delete wav;
	}
}
