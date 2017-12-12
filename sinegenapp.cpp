// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/fs_zip.h"

#include "sinegenapp.h"

IMPLEMENT_APP( SineGenApp )

IMPLEMENT_CLASS( SineGenApp, wxApp )


BEGIN_EVENT_TABLE( SineGenApp, wxApp )
	EVT_IDLE( SineGenApp::OnIdle )
END_EVENT_TABLE()


SineGenApp::SineGenApp()
{
    Init();
}


void SineGenApp::Init()
{
}

bool SineGenApp::OnInit()
{    
	wxXmlResource::Get()->InitAllHandlers();
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxXmlResource::Get()->Load(_T("sinegen.xrc"));

	//portaudio::AutoSystem autoSys;
	//portaudio::System &sys = portaudio::System::instance();


#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
	SineGen* mainWindow = new SineGen(NULL);
	/* int returnValue = */ mainWindow->ShowModal();

	mainWindow->Destroy();
	// A modal dialog application should return false to terminate the app.
	return false;
}


int SineGenApp::OnExit()
{    
	return wxApp::OnExit();
}


void SineGenApp::OnIdle( wxIdleEvent& event )
{
}


