#pragma once
#include "wx/stc/stc.h"
#include "languageSettings.h"

class Editor : public wxStyledTextCtrl {
public:
    Editor(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxSTCNameStr);

    ~Editor();

    void OnOpenFile(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);
    void OnHighlightLang(wxCommandEvent& event);
    const wxString GetFilepath();
    bool IsFilepathEmpty();
    const LanguageInfo GetLanguageInfo();
    
private:
    int DetermineLanguage(const wxString& filename);
    bool SetLanguageSettings(const int& index);

private:
    wxString m_filepath;

    // language properties
    LanguageInfo const* m_language = nullptr;
};
