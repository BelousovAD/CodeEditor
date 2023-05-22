#define _CRT_SECURE_NO_DEPRECATE
#include "ProjectExplorer.h"

#include "wx/dir.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/textfile.h"

#include "defsexternal.h"
#include "TreeItemData.h"

ProjectExplorer::ProjectExplorer(wxWindow* parent, wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
    : wxTreeCtrl(parent, id, pos, size, style, validator, name) {
    m_parent = parent;

    Bind(wxEVT_MENU, &ProjectExplorer::RedirectToParent, this, ID_Open);
    Bind(wxEVT_MENU, &ProjectExplorer::RedirectToParent, this, ID_SetFileAsMain);
    Bind(wxEVT_MENU, &ProjectExplorer::RedirectToParent, this, ID_FileUp);
    Bind(wxEVT_MENU, &ProjectExplorer::RedirectToParent, this, ID_FileDown);
    Bind(wxEVT_MENU, &ProjectExplorer::RedirectToParent, this, ID_Delete);
    Bind(wxEVT_MENU, &ProjectExplorer::RedirectToParent, this, ID_ExcludeFile);
    Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &ProjectExplorer::OnItemRightClick, this, ID_ProjectExplorer);
    Bind(wxEVT_TREE_ITEM_MENU, &ProjectExplorer::OnItemMenu, this, ID_ProjectExplorer);
}

ProjectExplorer::~ProjectExplorer() {
    this->DeleteAllItems();
}

void ProjectExplorer::OnItemRightClick(wxTreeEvent& event) {
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk() || m_root == itemId) {
        return;
    }
    this->SelectItem(itemId);
}

void ProjectExplorer::OnItemMenu(wxTreeEvent& event) {
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk() || m_root == itemId) {
        return;
    }

    wxPoint clientpt = event.GetPoint();

    wxMenu itemMenu;
    itemMenu.Append(ID_Open, _("&Open"));
    itemMenu.Append(ID_SetFileAsMain, _("&Set as main"));
    itemMenu.Append(ID_FileUp, _("&Move up"));
    itemMenu.Append(ID_FileDown, _("&Move down"));
    itemMenu.AppendSeparator();
    itemMenu.Append(ID_ExcludeFile, _("&Exclude"));
    itemMenu.Append(ID_Delete, _("&Delete"));
    PopupMenu(&itemMenu, clientpt);
}

void ProjectExplorer::RedirectToParent(wxCommandEvent& event) {
    wxTreeItemId itemId = this->GetFocusedItem();
    if (!itemId.IsOk()) {
        return;
    }
    TreeItemData* itemData = (TreeItemData*)GetItemData(itemId);
    event.SetInt(itemData->getIndex());
    event.Skip();
}

void ProjectExplorer::ConstructTree(const ProjectInfo& projectInfo) {
    m_root = this->AddRoot(wxFileNameFromPath(projectInfo.filepath));

    for (int i = 0; i < projectInfo.files.size(); i++) {
        wxTreeItemId item = this->AppendItem(m_root, projectInfo.files[i], -1, -1, new TreeItemData(i));
        if (projectInfo.files[i] == projectInfo.mainFile) {
            SetItemTextColour(item, *wxRED);
            SetItemBackgroundColour(item, *wxLIGHT_GREY);
        }
    }
}