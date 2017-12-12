#ifndef _SINEGEN_H_
#define _SINEGEN_H_

#include "wx/xrc/xmlres.h"
#include "wx/notebook.h"
#include "paudiocl.h"
#include "SignalView.h"

#define ID_SINEGEN 10000
#define SYMBOL_SINEGEN_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_SINEGEN_TITLE _("SineGen")
#define SYMBOL_SINEGEN_IDNAME ID_SINEGEN
#define SYMBOL_SINEGEN_SIZE wxSize(600, 500)
#define SYMBOL_SINEGEN_POSITION wxDefaultPosition

class FunctionGenerator;
class CaptureFunction;
class Int24;
class unknown;

class SineGen: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SineGen )
    DECLARE_EVENT_TABLE()

	void FillInOutDevices();
	void FillSampleFreqAndDepth(bool isInput);

	bool CreateOutputStream();
	bool CreateInputStream();

	bool DestroyOutputStream();
	bool DestroyInputStream();

	bool StartOutput();
	bool StopOutput();
	bool StartInput();
	bool StopInput();

	void UpdateSliderValue(wxTextCtrl* textCtrl, wxSlider *sliderCtrl);
	void UpdateCurrentGeneratorValue(wxTextCtrl* textCtrl, double value);
	void UpdateCurrentGeneratorValues();

public:
    SineGen();
    SineGen( wxWindow* parent, wxWindowID id = SYMBOL_SINEGEN_IDNAME, const wxString& caption = SYMBOL_SINEGEN_TITLE, const wxPoint& pos = SYMBOL_SINEGEN_POSITION, const wxSize& size = SYMBOL_SINEGEN_SIZE, long style = SYMBOL_SINEGEN_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SINEGEN_IDNAME, const wxString& caption = SYMBOL_SINEGEN_TITLE, const wxPoint& pos = SYMBOL_SINEGEN_POSITION, const wxSize& size = SYMBOL_SINEGEN_SIZE, long style = SYMBOL_SINEGEN_STYLE );

    ~SineGen();

    void Init();

    void CreateControls();

	void UpdateData(void* data, portaudio::SampleDataFormat format, int samples);

    void OnButtonOutputStartClick( wxCommandEvent& event );
    void OnButtonOutputStartUpdate( wxUpdateUIEvent& event );
    void OnButtonOutputStopClick( wxCommandEvent& event );
    void OnButtonOutputStopUpdate( wxUpdateUIEvent& event );

    void OnButtonAddSineClick( wxCommandEvent& event );
    void OnButtonDelSineClick( wxCommandEvent& event );
    void OnButtonSaveWavClick( wxCommandEvent& event );


    void OnButtonInputStartClick( wxCommandEvent& event );
    void OnButtonInputStartUpdate( wxUpdateUIEvent& event );
    void OnButtonInputStopClick( wxCommandEvent& event );
    void OnButtonInputStopUpdate( wxUpdateUIEvent& event );

	void OnChoiceOutDeviceSelected( wxCommandEvent& event );
	void OnChoiceInDeviceSelected( wxCommandEvent& event );
    void OnChoiceOutUpdate( wxUpdateUIEvent& event );
    void OnChoiceInUpdate( wxUpdateUIEvent& event );
	void OnWaveformCheckboxClicked( wxCommandEvent& event );
	void OnSineListBoxSelectChanged( wxCommandEvent& event );

	void OnSliderAmpScrollCmd( wxScrollEvent& event );
	void OnSliderFreqScrollCmd( wxScrollEvent& event );
	void OnSliderPhaseScrollCmd( wxScrollEvent& event );

	void OnAmplitudeTextEnter( wxCommandEvent& event );
	void OnFrequencyTextEnter( wxCommandEvent& event );
	void OnPhaseTextEnter( wxCommandEvent& event );

    void OnCurrentValueUpdate( wxUpdateUIEvent& event );

	void OnSpinCtrlMaxScaleChange( wxSpinEvent& event );
	void OnSpinCtrlMinScaleChange( wxSpinEvent& event );


    wxBitmap GetBitmapResource( const wxString& name );
    wxIcon GetIconResource( const wxString& name );
    static bool ShowToolTips();
private:
	PaudioCl* m_PaudioCl;

	wxChoice *m_choiceOutputDevice;
	wxChoice *m_choiceOutputFreq;
	wxChoice *m_choiceOutputDepth;

	wxChoice *m_choiceInputDevice;
	wxChoice *m_choiceInputFreq;
	wxChoice *m_choiceInputDepth;

	wxChoice *m_choiceInputFFTSize;
	wxChoice *m_choiceInputWndFunc;

	wxCheckBox *m_checkBoxWaveform;

	wxListBox *m_componentsListBox;

	wxTextCtrl* m_newSineFreq;
	wxTextCtrl* m_newSineAmpl;
	wxTextCtrl* m_newSinePhase;

	wxSlider *m_frequencySlider;
	wxSlider *m_amplitudeSlider;
	wxSlider *m_phaseSlider;

	wxTextCtrl* m_curSineFreq;
	wxTextCtrl* m_curSineAmpl;
	wxTextCtrl* m_curSinePhase;

	bool m_valueChangedFromCode;

	wxSpinCtrl* m_scaleMaxValue;
	wxSpinCtrl* m_scaleMinValue;

	SignalView*	m_signalPanel;

	portaudio::MemFunCallbackStream<FunctionGenerator>* m_outputStream;
	FunctionGenerator* m_functionGenerator;

	portaudio::MemFunCallbackStream<CaptureFunction>* m_inputStream;
	CaptureFunction* m_captureFunction;
};

#endif // _SINEGEN_H_
