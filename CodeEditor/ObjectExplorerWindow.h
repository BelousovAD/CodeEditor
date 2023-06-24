#pragma once
#include "wx/dialog.h"

#include "ObjectExplorer.h"
#include "ProjectInfo.h"

class ObjectExplorerWindow : public wxDialog {
public:
    ObjectExplorerWindow(wxWindow* parent, const ProjectInfo& projectInfo,
        wxWindowID id = ID_ObjectExplorerWindow,
        const wxString& title = "Object explorer",
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString& name = wxASCII_STR(wxDialogNameStr));
    ~ObjectExplorerWindow();

private:
    void OnSize(wxSizeEvent& event);

    ObjectExplorer* m_ObjectExplorer = nullptr;
};