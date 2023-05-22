#pragma once
#include <vector>
#include "wx/string.h"

struct ProjectInfo {
    wxString directory = "";
    wxString filepath = "";
    std::vector<wxString> imports;
    std::vector<wxString> files;
    wxString mainFile = "";
    wxString execFile = "";
    int target = 0;
    bool ProjectHaveChanges = false;

    bool HasFile(const wxString& filename) {
        std::vector<wxString>::iterator iter;
        iter = std::find(files.begin(), files.end(), filename);
        return iter != files.end();
    }

    bool HasImport(const wxString& filename) {
        std::vector<wxString>::iterator iter;
        iter = std::find(imports.begin(), imports.end(), filename);
        return iter != imports.end();
    }
};