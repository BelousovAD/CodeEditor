#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "Editor.h"

Editor::Editor(wxWindow* parent, wxWindowID id,
    const wxPoint& pos,
    const wxSize& size, long style,
    const wxString& name)
    : wxStyledTextCtrl(parent, id, pos, size, style) {
    StyleSetSpec(wxSTC_STYLE_DEFAULT, "face:Consolas,fore:#F1F1F1,back:#1E1E1E,size:13");
    SetLanguageSettings(NULL);
    Bind(wxEVT_MENU, &Editor::OnOpenFile, this, ID_Open);
    Bind(wxEVT_MENU, &Editor::OnSaveFile, this, ID_Save);
    Bind(wxEVT_MENU, &Editor::OnHighlightLang, this, ID_HIGHLIGHTFIRST, ID_HIGHLIGHTLAST);
}

Editor::~Editor() {
}

void Editor::OnHighlightLang(wxCommandEvent& event) {
    SetLanguageSettings(event.GetId() - ID_HIGHLIGHTFIRST);
}

int Editor::DetermineLanguage(const wxString& filename) {

    LanguageInfo const* m_language;

    // determine language from filepatterns
    for (int i = 0; i < g_LanguageSettings.size(); i++) {
        m_language = &g_LanguageSettings[i];
        wxString filepattern = m_language->filepattern;
        while (!filepattern.empty()) {
            wxString current = filepattern.BeforeFirst(' ');
            if (filename.AfterLast('.') == current) {
                return i;
            }
            filepattern = filepattern.AfterFirst(' ');
        }
    }
    return 0;
}

bool Editor::SetLanguageSettings(const int& index) {
    StyleClearAll();

    SetMarginType(ID_MARGIN_LINENUMBER, wxSTC_MARGIN_NUMBER);
    StyleSetSpec(wxSTC_STYLE_LINENUMBER, "face:Consolas,fore:#F1F1F1,back:#1E1E1E,size:13");
    SetMarginWidth(ID_MARGIN_LINENUMBER, 40);

    SetMarginType(ID_MARGIN_DIVIDER, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth(ID_MARGIN_DIVIDER, 0);
    SetMarginSensitive(ID_MARGIN_DIVIDER, false);

    m_language = &g_LanguageSettings[index];
    SetLexer(m_language->lexer);
    for (int i = 0; i < m_language->styles.size(); i++) {
        const Style currentStyle = m_language->styles[i];
        StyleSetSpec(currentStyle.type, currentStyle.spec);
    }
    for (int i = 0; i < m_language->keywordsStyles.size(); i++) {
        const KeywordsStyle currentStyle = m_language->keywordsStyles[i];
        SetKeyWords(currentStyle.type, currentStyle.wordsString);
    }

    // set spaces and indentation
    SetTabWidth(4);
    SetUseTabs(false);
    SetTabIndents(true);
    SetBackSpaceUnIndents(true);
    SetIndent(4);

    return true;
}

const LanguageInfo Editor::GetLanguageInfo() {
    return *m_language;
}

void Editor::OnOpenFile(wxCommandEvent& event) {
    m_filepath = event.GetString();

    wxStyledTextCtrl::LoadFile(m_filepath);

    SetLanguageSettings(DetermineLanguage(m_filepath));
}

void Editor::OnSaveFile(wxCommandEvent& event) {
    if (!IsModified()) {
        return;
    }
    else if (m_filepath.IsEmpty() || !wxFileName::FileExists(m_filepath)) {
        wxMessageBox("filename is empty or file does not exist", "Save file", wxOK, this);
    }
    else {
        wxStyledTextCtrl::SaveFile(m_filepath);
    }
}

const wxString Editor::GetFilepath() {
    return m_filepath;
}

bool Editor::IsFilepathEmpty() {
    return m_filepath.IsEmpty();
}