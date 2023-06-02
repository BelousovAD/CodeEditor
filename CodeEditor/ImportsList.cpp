#include "ImportsList.h"

#include "wx/menu.h"

ImportsList::ImportsList(wxWindow* parent,
	wxWindowID winid,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name)
	: wxListView(parent, winid, pos, size, style, validator, name) {
    Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ImportsList::OnItemRightClick, this, ID_ImportsList);
    Bind(wxEVT_MENU, &ImportsList::RedirectToParent, this, ID_DirAppend);
    Bind(wxEVT_MENU, &ImportsList::RedirectToParent, this, ID_DirUp);
    Bind(wxEVT_MENU, &ImportsList::RedirectToParent, this, ID_DirDown);
    Bind(wxEVT_MENU, &ImportsList::RedirectToParent, this, ID_DirDelete);
}

ImportsList::~ImportsList() {
	this->DeleteAllItems();
	this->DeleteAllColumns();
}

void ImportsList::OnItemRightClick(wxListEvent& event) {
    long index = event.GetIndex();
    if (index == -1) {
        return;
    }
    this->Select(index);
    OnItemMenu(event);
}

void ImportsList::OnItemMenu(wxListEvent& event) {
    wxPoint clientpt = event.GetPoint();

    wxMenu itemMenu;
    itemMenu.Append(ID_DirUp, _("&Move up"));
    itemMenu.Append(ID_DirDown, _("&Move down"));
    itemMenu.AppendSeparator();
    itemMenu.Append(ID_DirDelete, _("&Delete"));
    PopupMenu(&itemMenu, clientpt);
}

void ImportsList::RedirectToParent(wxCommandEvent& event) {
    event.SetInt(this->GetFirstSelected());
    event.Skip();
}

void ImportsList::ConstructList(const std::vector<wxString>& imports) {
    this->AppendColumn(_("Path"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    for (int i = imports.size() - 1; i >= 0 ; --i) {
        this->InsertItem(0, imports[i]);
    }
}