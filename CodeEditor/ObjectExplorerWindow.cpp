#include "ObjectExplorerWindow.h"

#include <vector>
#include "wx/button.h"
#include "wx/filename.h"
#include "wx/sizer.h"
#include "wx/textfile.h"

ObjectExplorerWindow::ObjectExplorerWindow(wxWindow* parent,
	const ProjectInfo& projectInfo,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxDialog (parent, id, title, pos, size, style, name) {

	m_ObjectExplorer = new ObjectExplorer(this);

	for (int i = 0; i < projectInfo.files.size(); i++) {
		std::vector<std::pair<wxString, int>> objects;

		wxString cur_filepath = projectInfo.directory +
			wxFileName::GetPathSeparator() +
			projectInfo.files[i];

		if (!wxFileExists(cur_filepath)) {
			continue;
		}

		wxTextFile File(cur_filepath);
		if (File.Open()) {
			int j = 1;
			for (wxString line = File.GetFirstLine(); !File.Eof(); line = File.GetNextLine()) {
				line.Trim(false);
				if (line.BeforeFirst(' ') != "PROCEDURE") {
					++j;
					continue;
				}
				line = line.AfterFirst(' ');
				std::pair<wxString, int> pair;

				if (line.Find('*') != -1) {
					++j;
					continue;
				}
				
				if (line.Find('{') == -1) {
					++j;
					continue;
				}

				line = line.BeforeFirst('}');

				pair.first = "PROCEDURE " + line + '}';
				pair.second = j;
				objects.push_back(pair);
				++j;
			}
			File.Close();
		}
		else {
			continue;
		}

		m_ObjectExplorer->AddList(objects, projectInfo.files[i]);
	}

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(m_ObjectExplorer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	SetSizerAndFit(topSizer);

	Bind(wxEVT_SIZE, &ObjectExplorerWindow::OnSize, this);
};

ObjectExplorerWindow::~ObjectExplorerWindow() {
	m_ObjectExplorer->Destroy();
	m_ObjectExplorer = nullptr;
};

void ObjectExplorerWindow::OnSize(wxSizeEvent& event) {
	wxSize size = this->GetClientSize();
	m_ObjectExplorer->SetSize(size.GetWidth() - 10, size.GetHeight() - 10);
	this->Refresh();
};