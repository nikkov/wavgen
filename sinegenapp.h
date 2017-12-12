#ifndef _SINEGENAPP_H_
#define _SINEGENAPP_H_


#include "wx/xrc/xmlres.h"
#include "wx/image.h"
#include "sinegen.h"

class SineGenApp: public wxApp
{    
    DECLARE_CLASS( SineGenApp )
    DECLARE_EVENT_TABLE()

public:
    SineGenApp();

    void Init();

    virtual bool OnInit();

    virtual int OnExit();

	void OnIdle( wxIdleEvent& event );
};

DECLARE_APP(SineGenApp)

#endif    // _SINEGENAPP_H_
