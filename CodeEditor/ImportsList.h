#pragma once
#include "wx/listctrl.h"
#include <vector>
#include "wx/string.h"

#include "defsexternal.h"

class ImportsList : public wxListView {
public:
    ImportsList(wxWindow* parent,
        wxWindowID winid = ID_ImportsList,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxLC_REPORT,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxListCtrlNameStr));
    ~ImportsList();

    void ConstructList(const std::vector<wxString> &imports);

private:
    void OnItemRightClick(wxListEvent& event);
    void OnItemMenu(wxListEvent& event);

    void RedirectToParent(wxCommandEvent& event);
};