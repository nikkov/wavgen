#ifndef _SIGNAL_VIEW_H_
#define _SIGNAL_VIEW_H_

#include <wx/wx.h>
#include <wx/defs.h>
#include "wx/gbsizer.h"

#include "portaudiocpp/PortAudioCpp.hxx"
//#include "fft_wrapper.h"
#include "coredsp.h"


class SignalView :
	public wxPanel
{
	DECLARE_DYNAMIC_CLASS( MIPalette )
	DECLARE_EVENT_TABLE()
public:
	SignalView();
	SignalView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER );

	bool Create( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER );

	~SignalView();
	void Init();
	void CreateControls();
	void SetSignalParams(int length, float sampleFreq, namespace_utilfunc::WindowFuncType wndFunc);
	//void SetYScale(float scale);
	void SetYMaxScale(float scale);
	void SetYMinScale(float scale);

	virtual void UpdateData(void* data, portaudio::SampleDataFormat format, int samples);
	void ClearDisplay();
	void ShowWaveform(bool show)
	{
		wxCriticalSectionLocker locker(m_csDC);
		m_showWaveform = show;
		m_needUpdateScale = true;
	}

private:

	virtual void UpdateSpectrum(void* data, portaudio::SampleDataFormat format, int samples);
	virtual void UpdateSignal(void* data, portaudio::SampleDataFormat format, int samples);

	template<typename T>
	void CopyDataFromBufferInternal(void* data, int samples, float kf);

	void CopyDataFromBuffer(void* data, portaudio::SampleDataFormat format, int samples, bool needNorm);

	wxBitmap*	m_bitmap;
	wxMemoryDC*	m_memDC;
	bool		m_needUpdateScale;
	bool		m_showWaveform;

	void CreateBitmap();
	void ClearBitmap(bool clearScale);

	void OnPaint( wxPaintEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnEraseBackground(wxEraseEvent &event);

	void DrawSpectrum(bool drawScale);
	void DrawWaveform(bool drawScale);

	virtual void DrawScale();
	virtual void DrawGrid();

	//FFTWrapper m_fft;
	float *m_signalBuffer;
	unsigned long m_fft_legth;

	//Signal params
	float m_sampleFreq;
	float m_yScaleMax;
	float m_yScaleMin;
	float m_scaleYStep;
	namespace_utilfunc::WindowFuncType m_wndFunc;

	wxCriticalSection m_csDC;

};

#endif //_SIGNAL_VIEW_H_