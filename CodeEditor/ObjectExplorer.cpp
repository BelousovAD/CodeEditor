#include "ObjectExplorer.h"

ObjectExplorer::ObjectExplorer(wxWindow* parent,
	wxWindowID winid,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name)
	: wxListView(parent, winid, pos, size, style, validator, name) {
	this->AppendColumn(_("Object"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	this->AppendColumn(_("File"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
	this->AppendColumn(_("Line"), wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
}

ObjectExplorer::~ObjectExplorer() {
	this->DeleteAllItems();
	this->DeleteAllColumns();
}

void ObjectExplorer::AddList(const std::vector<std::pair<wxString, int>>& objects, const wxString& filename) {
	for (int i = objects.size() - 1; i >= 0; --i) {
		this->InsertItem(0, objects[i].first);
		this->SetItem(0, 1, filename);
		this->SetItem(0, 2, wxString::Format("%d", objects[i].second));
	}
	this->SetColumnWidth(0, wxLIST_AUTOSIZE);
	this->SetColumnWidth(1, wxLIST_AUTOSIZE);
	this->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
}