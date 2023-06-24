#pragma once
#include "wx/listctrl.h"

#include "defsexternal.h"

class ObjectExplorer : public wxListView {
public:
	ObjectExplorer(wxWindow* parent,
        wxWindowID winid = ID_ObjectExplorer,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxLC_REPORT,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxListCtrlNameStr));
	~ObjectExplorer();

    void AddList(const std::vector<std::pair<wxString, int>>& objects, const wxString& filename);
};