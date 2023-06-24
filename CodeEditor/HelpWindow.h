#pragma once
#include "wx/dialog.h"

#include "defsexternal.h"
#include "wx/textctrl.h"

class HelpWindow : public wxDialog {
public:
    HelpWindow(wxWindow* parent,
        wxWindowID id = ID_HelpWindow,
        const wxString& title = "Help",
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString& name = wxASCII_STR(wxDialogNameStr));
    ~HelpWindow();

private:
    void OnSize(wxSizeEvent& event);

    wxTextCtrl* m_TextCtrl = nullptr;
};