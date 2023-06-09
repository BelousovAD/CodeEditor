#pragma once
#include "wx/treectrl.h"

#include "ProjectInfo.h"

class ProjectExplorer : public wxTreeCtrl {
public:
    ProjectExplorer(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxTreeCtrlNameStr));
    ~ProjectExplorer();

    void ConstructTree(const ProjectInfo& projectInfo);

private:
    void OnItemRightClick(wxTreeEvent& event);
    void OnItemMenu(wxTreeEvent& event);

    void RedirectToParent(wxCommandEvent& event);

private:
    wxTreeItemId m_root;
};