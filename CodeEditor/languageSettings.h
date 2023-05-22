#pragma once
#include "wx/stc/stc.h"
#include "defsexternal.h"
#include <vector>

struct Style {
    int type;
    wxString spec;
};

struct KeywordsStyle {
    int type;
    wxString wordsString;
};

struct LanguageInfo {
    wxString name;
    wxString filepattern;
    int lexer;
    const std::vector<Style> styles;
    const std::vector<KeywordsStyle> keywordsStyles;
};

extern const std::vector<LanguageInfo> g_LanguageSettings;