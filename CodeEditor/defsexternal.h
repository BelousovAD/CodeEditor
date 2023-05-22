#pragma once
#include <wx/defs.h>

enum {
    ID_MARGIN_LINENUMBER = 0,
    ID_MARGIN_DIVIDER,
    ID_MainWindow = wxID_HIGHEST + 1,
    ID_Notebook,
    ID_Editor,
    ID_ProjectExplorer,
    ID_HIGHLIGHTFIRST,
    ID_HIGHLIGHTLAST = ID_HIGHLIGHTFIRST + 30,
    ID_ProjectNew,
    ID_ProjectOpen,
    ID_ProjectSave,
    ID_ProjectClose,
    ID_New,
    ID_Save,
    ID_IncludeFile,
    ID_Open,
    ID_ExcludeFile,
    ID_SetFileAsMain,
    ID_FileUp,
    ID_FileDown,
    ID_Delete,
    ID_TargetMIN,
    ID_TargetMAX = ID_TargetMIN + 3,
};

const std::vector<wxString> Targets = {
    "Win32 Console Application(MS Visual C++ 6.0)",
    "Win32 Application (MS Visual C++ 6.0)",
    "Console Application (MinGW 3.1.0)",
    "Win32 Application(MinGW 3.1.0)"
};