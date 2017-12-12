// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/graphics.h>
#include "SignalView.h"
#include "Int24.h"


IMPLEMENT_DYNAMIC_CLASS( SignalView, wxPanel )


BEGIN_EVENT_TABLE( SignalView, wxPanel )
	EVT_PAINT( SignalView::OnPaint )
	EVT_SIZE( SignalView::OnSize )
	EVT_ERASE_BACKGROUND( SignalView::OnEraseBackground )
END_EVENT_TABLE()


const int view_shift_x = 80;
const int view_shift_y = 50;
const int view_margin = 10;


SignalView::SignalView(void)
{
	Init();
}

SignalView::SignalView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}

bool SignalView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	wxPanel::Create( parent, id, pos, size, style );
	CreateControls();
	Centre();
	return true;
}

SignalView::~SignalView(void)
{
	if(m_memDC) delete m_memDC;
	if(m_bitmap) delete m_bitmap;
	if(m_signalBuffer) delete m_signalBuffer;
}

void SignalView::Init()
{
	m_showWaveform = false;
	m_needUpdateScale = false;
	m_scaleYStep = 10.0f;
	m_bitmap = NULL;
	m_memDC = NULL;
	m_fft_legth = 0;
	m_signalBuffer = NULL;
	m_yScaleMax = 10.0f;
	m_yScaleMin = -120.0f;
	m_wndFunc = namespace_utilfunc::WND_HANNING;
}

void SignalView::CreateControls()
{    
	CreateBitmap();
	//DrawScale();
	//DrawGrid();
}

void SignalView::CreateBitmap()
{
	wxCriticalSectionLocker locker(m_csDC);

	if(m_memDC) delete m_memDC;
	if(m_bitmap) delete m_bitmap;

	wxSize dcSize = GetClientSize();
	m_memDC = new wxMemoryDC();
	m_bitmap = new wxBitmap(dcSize.GetWidth(), dcSize.GetHeight());
	m_memDC->SelectObject(*m_bitmap);
	ClearBitmap(true);
	DrawScale();
	DrawGrid();
}

void SignalView::OnPaint( wxPaintEvent& event )
{
	wxCriticalSectionLocker locker(m_csDC);
	if(m_memDC)
	{
		wxPaintDC dc(this);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.Blit(this->GetClientRect().GetTopLeft(),
			this->GetClientRect().GetSize(), 
			m_memDC, wxPoint(0, 0));
	}
}

void SignalView::ClearBitmap(bool clearScale)
{
	wxColour clr = this->GetParent()->GetBackgroundColour();
	wxBrush palBrush1(clr);
	m_memDC->SetBrush(palBrush1);
	m_memDC->SetPen(clr);
	if(clearScale)
		m_memDC->DrawRectangle(0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());
	else
		m_memDC->DrawRectangle(view_shift_x + 1, view_margin + 1, 
			m_bitmap->GetWidth() - view_shift_x - view_margin - 2,
			m_bitmap->GetHeight() - view_shift_y - view_margin - 2);
}

void SignalView::OnSize( wxSizeEvent& event )
{
	CreateBitmap();
	{
		wxCriticalSectionLocker locker(m_csDC);
		if(m_showWaveform)
			DrawWaveform(true);
		else
			DrawSpectrum(true);
	}
	Refresh();
}

void SignalView::DrawScale()
{
	wxColour bckColor = this->GetParent()->GetBackgroundColour();

	//draw frame
	m_memDC->SetPen(*wxGREY_PEN);
	wxBrush bckBrush(bckColor);
	m_memDC->SetBrush(bckBrush);

	wxRect drawRect(view_shift_x, 
			view_margin, 
			m_bitmap->GetWidth() - view_margin - view_shift_x,
			m_bitmap->GetHeight() - view_margin - view_shift_y);

	m_memDC->DrawRectangle(drawRect);

	m_memDC->SetFont( wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, false) );
	wxCoord length;
    wxCoord height;
    m_memDC->GetTextExtent( wxT("0"), &length, &height);

	int tickNumber = (int)((float)(m_bitmap->GetHeight() - view_margin - view_shift_y) / ((float)height * 1.25f));

	m_scaleYStep = (m_yScaleMax - m_yScaleMin) / (float)tickNumber;
	if(m_scaleYStep < 3)
		m_scaleYStep = 1;
	else
		if(m_scaleYStep < 30)
			m_scaleYStep = 10;
		else
			if(m_scaleYStep < 300)
				m_scaleYStep = 100;


	//val to Y
	float kfY = (float)(drawRect.GetHeight() - 2) / (m_yScaleMax - m_yScaleMin);
	for(float val = m_yScaleMax; val >= m_yScaleMin; val -= m_scaleYStep)
	{
		float y = (m_yScaleMax - val) * kfY + view_margin;
		m_memDC->DrawLine(drawRect.GetLeft() - 3, y, drawRect.GetLeft(), y);
		wxString valText = wxString::Format(wxT("%.0f"), val);
		m_memDC->GetTextExtent( valText, &length, &height);
		m_memDC->DrawText(valText, drawRect.GetLeft() - 4 - length, y - height / 2);
	}
	m_needUpdateScale = false;
}

void SignalView::DrawGrid()
{
	if(m_memDC == NULL)
		return;

	m_memDC->SetPen(*wxGREY_PEN);
	wxRect drawRect(view_shift_x, 
			view_margin, 
			m_bitmap->GetWidth() - view_margin - view_shift_x,
			m_bitmap->GetHeight() - view_margin - view_shift_y);

	float kfY = (float)(drawRect.GetHeight() - 2) / (m_yScaleMax - m_yScaleMin);
	for(float val = m_yScaleMax; val >= m_yScaleMin; val -= m_scaleYStep)
	{
		float y = (m_yScaleMax - val) * kfY + view_margin;
		m_memDC->DrawLine(drawRect.GetLeft(), y, drawRect.GetRight(), y);
	}

	float scaleX = m_sampleFreq / 2.0f;
	float kx = (float)drawRect.GetWidth() / (float)scaleX;
	m_memDC->SetPen(*wxGREY_PEN);
	for(float x = 1000.0f; x < scaleX; x += 1000.0f)
	{
		float x1 = view_shift_x + x * kx;
		m_memDC->DrawLine((int)x1, drawRect.GetTop(), (int)x1, drawRect.GetBottom());
	}
}



void SignalView::DrawSpectrum(bool drawScale)
{
	if(m_memDC == NULL)
		return;

	if(drawScale || m_needUpdateScale)
		DrawScale();

	if(m_signalBuffer != NULL)
	{
		wxRect drawRect(view_shift_x + 1,
				view_margin + 1,
				m_bitmap->GetWidth() - view_margin - view_shift_x - 2,
				m_bitmap->GetHeight() - view_margin - view_shift_y - 2);

		m_memDC->SetClippingRegion(drawRect);

		int yCenter = drawRect.GetBottom();
		//float *showData = m_signalBuffer + m_fft.GetLength();
		float *showData = m_signalBuffer + m_fft_legth;

		wxPen redPen = wxPen(*wxStockGDI::GetColour(wxStockGDI::COLOUR_RED));

		float kfY = (float)(drawRect.GetHeight()) / (m_yScaleMax - m_yScaleMin);

		int spLineNum = m_fft_legth / 2;//m_fft.GetLength() / 2;

		float kx = (float)drawRect.GetWidth() / (float)spLineNum;

		int valY = (m_yScaleMax - showData[0]) * kfY + view_margin;

		for(int i = 1; i < spLineNum; i++)
		{
			int val0 = (m_yScaleMax - showData[i]) * kfY + view_margin;

			float x1 = drawRect.GetLeft() + kx * (float)(i - 1);
			float x2 = drawRect.GetLeft() + kx * (float)i;

			m_memDC->SetPen(redPen);
			m_memDC->DrawLine((int)x1, valY, (int)x2, val0);

			valY = val0;
		}

	}
	DrawGrid();
}

void SignalView::DrawWaveform(bool drawScale)
{
	if(m_memDC == NULL)
		return;

	if(drawScale || m_needUpdateScale)
		DrawScale();

	if(m_signalBuffer != NULL)
	{
		wxRect drawRect(view_shift_x + 1,
				view_margin + 1,
				m_bitmap->GetWidth() - view_margin - view_shift_x - 2,
				m_bitmap->GetHeight() - view_margin - view_shift_y - 2);

		m_memDC->SetClippingRegion(drawRect);

		int yCenter = drawRect.GetBottom();
		float *showData = m_signalBuffer;

		wxPen redPen = wxPen(*wxStockGDI::GetColour(wxStockGDI::COLOUR_RED));

		int shiftY = drawRect.GetHeight()/2 + view_margin;
		float kfY = (float)(drawRect.GetHeight()) / (m_yScaleMax - m_yScaleMin) * 2.0f;

		int spLineNum = m_fft_legth; //m_fft.GetLength();

		float kx = (float)drawRect.GetWidth() / (float)spLineNum;

		int valY = shiftY + showData[0] * kfY;

		for(int i = 1; i < spLineNum; i++)
		{
			//int val0 = (m_yScaleMax - showData[i]) * kfY + view_margin;
			int val0 = shiftY + showData[i] * kfY;

			float x1 = drawRect.GetLeft() + kx * (float)(i - 1);
			float x2 = drawRect.GetLeft() + kx * (float)i;

			m_memDC->SetPen(redPen);
			m_memDC->DrawLine((int)x1, valY, (int)x2, val0);

			valY = val0;
		}

	}
	DrawGrid();
}


template<typename T>
void SignalView::CopyDataFromBufferInternal(void* data, int samples, float kf)
{
	T* sdata = (T*)data;
	int j = 0;
	for(int i = 0; i < 2 * samples; i += 2)
		m_signalBuffer[j++] = (float)sdata[i] * kf;
}

void SignalView::CopyDataFromBuffer(void* data, portaudio::SampleDataFormat format, int samples, bool needNorm)
{
	float kf = needNorm ? (float)(100.0 / GetMaxValueForFormat(format)) : 1.0f;
	switch(format)
	{
		case portaudio::INT16:
			CopyDataFromBufferInternal<short>(data, samples, kf);
			break;
		case portaudio::INT24:
			CopyDataFromBufferInternal<Int24>(data, samples, kf);
			break;
		case portaudio::INT32:
			CopyDataFromBufferInternal<int>(data, samples, kf);
			break;
		case portaudio::FLOAT32:
			CopyDataFromBufferInternal<float>(data, samples, kf);
			break;
	}
}


void SignalView::UpdateSpectrum(void* data, portaudio::SampleDataFormat format, int samples)
{
	if(m_memDC && m_signalBuffer && m_fft_legth/*m_fft.GetLength()*/ == samples)
	{
		{
			wxCriticalSectionLocker locker(m_csDC);
			CopyDataFromBuffer(data, format, samples, false);
			//float *showData = m_signalBuffer + m_fft.GetLength();
			float *showData = m_signalBuffer + m_fft_legth;
			
			//m_fft.FFT(m_signalBuffer, showData, m_wndFunc);

			float *data = new float[m_fft_legth];
			for ( ulong i = 0; i < m_fft_legth; i++ )
			{
				data[i] =(REAL)( 100. * sin( 2. * M_PI * 1000. * (double)i / m_sampleFreq ) ) ;
			}


			//namespace_utilfunc::window_func_real(data, m_wndFunc, 1, m_fft_legth);
			//namespace_utilfunc::window_func_real(m_signalBuffer, m_wndFunc, 1, m_fft_legth);

			namespace_correlation::fft_correlation(data, m_signalBuffer, m_fft_legth, namespace_fft::FftRadix2);

			for ( ulong i = 0; i < m_fft_legth / 2; i++ )
			{
				showData[i] = m_signalBuffer[i];
			}
			delete data;

/*
			namespace_utilfunc::window_func_real(m_signalBuffer, m_wndFunc, 1, m_fft_legth);
			namespace_fft::RealFFt(m_signalBuffer, m_fft_legth, -1, namespace_fft::FftRadix2);
			int half_length = m_fft_legth / 2;
			int j = 0;
			float val = m_signalBuffer[0] / (float)half_length;
			float max_val = GetMaxValueForFormat(format);
			val /= max_val;
			showData[j++] = 20.0*log10(val);

			for(int i = 1; i < m_fft_legth; i += 2)
			{
				float val = sqrt(m_signalBuffer[i]*m_signalBuffer[i] + m_signalBuffer[1 + i]*m_signalBuffer[1 + i]) / (float)half_length;
				val /= max_val;
				showData[j++] = 20.0*log10(val);
			}
*/			
			ClearBitmap(false);
			DrawSpectrum(false);
		}
		Refresh();
	}
}

void SignalView::UpdateSignal(void* data, portaudio::SampleDataFormat format, int samples)
{
	if(m_memDC && m_signalBuffer)
	{
			wxCriticalSectionLocker locker(m_csDC);
			CopyDataFromBuffer(data, format, samples, true);
			ClearBitmap(false);
			DrawWaveform(false);
	}
}

void SignalView::UpdateData(void* data, portaudio::SampleDataFormat format, int samples)
{
	if(m_memDC && m_signalBuffer && m_fft_legth/*m_fft.GetLength()*/ == samples)
	{
		{
			wxCriticalSectionLocker locker(m_csDC);
			if(m_showWaveform)
				UpdateSignal(data, format, samples);
			else
				UpdateSpectrum(data, format, samples);
		}
		Refresh();
	}
}

void SignalView::ClearDisplay()
{
	if(m_memDC)
	{
		ClearBitmap(false);
		Refresh();
	}
}

void SignalView::OnEraseBackground(wxEraseEvent &event)
{
}

void SignalView::SetSignalParams(int length, float sampleFreq, namespace_utilfunc::WindowFuncType wndFunc)
{
	if(m_fft_legth/*m_fft.GetLength()*/ != length)
	{
		//m_fft.Resize(length);
		if(m_signalBuffer)
			delete m_signalBuffer;
		m_fft_legth = length;
		m_signalBuffer = NULL;
		//if(m_fft.GetLength() == length)
		//	m_signalBuffer = new float[2 * length];
		m_signalBuffer = new float [2 * m_fft_legth];
	}
	m_sampleFreq = sampleFreq;
	m_wndFunc = wndFunc;
}

void SignalView::SetYMaxScale(float scale) 
{ 
	m_yScaleMax = scale; 
	CreateBitmap();
	//m_needUpdateScale = true;
	//Refresh();
}

void SignalView::SetYMinScale(float scale) 
{ 
	m_yScaleMin = scale; 
	CreateBitmap();
	//m_needUpdateScale = true;
	//Refresh();
}

