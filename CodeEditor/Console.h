#pragma once
#include "wx/listbox.h"
#include "defsexternal.h"

class Console : public wxListBox {
public:
    Console(wxWindow* parent, wxWindowID id = ID_Console,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = wxLB_HSCROLL,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxListBoxNameStr));
    ~Console();
    
    void ShowOutput(const wxString& cmd,
        const wxArrayString& output,
        const wxString& title,
        const wxString& appName);
};