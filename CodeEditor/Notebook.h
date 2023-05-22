#pragma once
#include "wx/aui/aui.h"

#include "Editor.h"

class Notebook : public wxAuiNotebook {
public:
    Notebook(wxWindow* parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxAUI_NB_DEFAULT_STYLE);

    ~Notebook();

    const bool IsIndexOfCurrentHighlightLanguage(const size_t& index);

    void OnSaveFile(wxCommandEvent& event);
    void OnHighlightLanguage(wxCommandEvent& event);

private:
    void OnFileOpen(wxCommandEvent& event);
    void OnNotebookPageCloseByNotebook(wxAuiNotebookEvent& event);
    void OnNotebookPageCloseByMainWindow(wxAuiNotebookEvent& event);
    void OnSavePointReached(wxStyledTextEvent& event);
    void OnSavePointLeft(wxStyledTextEvent& event);

    int IndexIfFileOpenedInNotebook(const wxString& filename);
    Editor* CreateEditor();
};