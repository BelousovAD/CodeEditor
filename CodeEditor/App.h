#pragma once
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif // !WX_PRECOMP

class App : public wxApp
{
public:
	bool OnInit() wxOVERRIDE;
};

wxDECLARE_APP(App);