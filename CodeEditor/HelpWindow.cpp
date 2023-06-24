#include "HelpWindow.h"

#include "wx/sizer.h"

HelpWindow::HelpWindow(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxDialog(parent, id, title, pos, size, style, name) {
	m_TextCtrl = new wxTextCtrl(this, wxID_ANY,
		UserManual,
		wxDefaultPosition,
		wxSize(500, 300),
		wxTE_READONLY | wxTE_MULTILINE);

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(m_TextCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	SetSizerAndFit(topSizer);

	Bind(wxEVT_SIZE, &HelpWindow::OnSize, this);
}

HelpWindow::~HelpWindow() {
	this->m_TextCtrl->Clear();
	this->m_TextCtrl->Destroy();
	m_TextCtrl = nullptr;
}

void HelpWindow::OnSize(wxSizeEvent& event) {
	wxSize size = this->GetClientSize();
	m_TextCtrl->SetSize(size.GetWidth() - 10, size.GetHeight() - 10);
	this->Refresh();
};