#include "MainWindow.h"
#include "defsexternal.h"

#include "wx/dir.h"
#include "wx/textfile.h"

MainWindow::MainWindow(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxFrame(nullptr, wxID_ANY, title, pos, size, style, name)
{
	m_AuiManager = new wxAuiManager(this);

	// seting default style of notebook
	m_NotebookStyle = wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
		wxAUI_NB_WINDOWLIST_BUTTON |
		wxBORDER_NONE;

	m_Notebook = CreateNotebook();
	m_ProjectExplorer = CreateProjectExplorer();
	
	SetMenuBar(CreateMenuBar());

	CreateStatusBar();
	GetStatusBar()->SetStatusText(_("Ready"));

	m_AuiManager->AddPane(m_Notebook, wxAuiPaneInfo().
		Name("Notebook0").
		CenterPane());
	m_AuiManager->AddPane(m_ProjectExplorer, wxAuiPaneInfo().
		Name("ProjectExplorer0").Caption(_("Project explorer")).
		Layer(0).Left().Row(0).Position(0).
		CloseButton(true).MaximizeButton(true));

	m_AuiManager->Update();

	Bind(wxEVT_MENU, &MainWindow::OnOpenFile, this, ID_Open);
	Bind(wxEVT_MENU, &MainWindow::OnSetFileAsMain, this, ID_SetFileAsMain);
	Bind(wxEVT_MENU, &MainWindow::OnFileUp, this, ID_FileUp);
	Bind(wxEVT_MENU, &MainWindow::OnFileDown, this, ID_FileDown);
	Bind(wxEVT_MENU, &MainWindow::OnExcludeFile, this, ID_ExcludeFile);
	Bind(wxEVT_MENU, &MainWindow::OnDeleteFile, this, ID_Delete);

	Bind(wxEVT_MENU, &MainWindow::OnCreateFile, this, ID_New);
	Bind(wxEVT_MENU, &Notebook::OnSaveFile, m_Notebook, ID_Save);
	Bind(wxEVT_MENU, &MainWindow::OnIncludeFile, this, ID_IncludeFile);
	Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainWindow::OnProjectNew, this, ID_ProjectNew);
	Bind(wxEVT_MENU, &MainWindow::OnProjectOpen, this, ID_ProjectOpen);
	Bind(wxEVT_MENU, &MainWindow::OnProjectSave, this, ID_ProjectSave);
	Bind(wxEVT_MENU, &MainWindow::OnProjectClose, this, ID_ProjectClose);
	Bind(wxEVT_MENU, &Notebook::OnHighlightLanguage, m_Notebook, ID_HIGHLIGHTFIRST, ID_HIGHLIGHTLAST);
	Bind(wxEVT_MENU, &MainWindow::OnSetTargetPlatform, this, ID_TargetMIN, ID_TargetMAX);

	Bind(wxEVT_CLOSE_WINDOW, &MainWindow::OnCloseWindow, this);

	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateFileMenu, this, ID_New, ID_IncludeFile);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateProjectMenu, this, ID_ProjectNew, ID_ProjectClose);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateTargetPlatformMenu, this, ID_TargetMIN, ID_TargetMAX);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateHighlightMenu, this, ID_HIGHLIGHTFIRST, ID_HIGHLIGHTLAST);
}

MainWindow::~MainWindow() {
	m_Notebook->Destroy();
	m_ProjectExplorer->Destroy();
	delete m_AuiManager;
}

void MainWindow::OnProjectNew(wxCommandEvent& event) {
	wxDirDialog newProjectDialog(this, _("Choose directory"),
		wxEmptyString,
		wxDD_DEFAULT_STYLE | wxDD_CHANGE_DIR);
	if (newProjectDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	if (m_projectInfo) {
		OnProjectClose(event);
	}
	else {
		m_projectInfo = new ProjectInfo();
	}

	wxString dirpath = newProjectDialog.GetPath();
	wxString filename;
	wxDir dir(dirpath);
	if (dir.GetFirst(&filename, "*.pro", wxDIR_FILES)) {
		wxMessageBox(_("Cannot create the project. "
			"This directory is used by another project."),
			_("Error"),
			wxOK, this);
		return;
	}
	else {
		wxString filepath = dirpath + wxFileName::GetPathSeparator() + "NewProject.pro";

		m_projectInfo->directory = dirpath;
		m_projectInfo->filepath = filepath;

		if (!CreateNewFile(filepath) ||
			!CreateNewFile(dirpath + wxFileName::GetPathSeparator() + "main.o")) {
			wxMessageBox(_("Cannot create project files in this directory."),
				_("Error"),
				wxOK, this);
			delete m_projectInfo;
			m_projectInfo = nullptr;
			return;
		}

		m_projectInfo->execFile = "NewProject.exe";
		m_projectInfo->files.push_back("main.o");
		m_projectInfo->mainFile = "main.o";
		m_projectInfo->target = NULL;
		m_projectInfo->ProjectHaveChanges = true;

		OnProjectSave(event);
		m_ProjectExplorer->ConstructTree(GetProjectInfo());
	}
}

void MainWindow::OnProjectSave(wxCommandEvent& event) {
	wxTextFile projectFile(m_projectInfo->filepath);
	if (projectFile.Open()) {
		projectFile.Clear();
		for (int i = 0; i < m_projectInfo->imports.size(); i++) {
			projectFile.AddLine("IMPORT=" + m_projectInfo->imports[i]);
		}
		projectFile.AddLine("MAIN=" + m_projectInfo->mainFile);
		projectFile.AddLine("EXEC=" + m_projectInfo->execFile);
		for (int i = 0; i < m_projectInfo->files.size(); i++) {
			projectFile.AddLine("FILE=" + m_projectInfo->files[i]);
		}
		projectFile.AddLine(wxString::Format("TARGET=%d", m_projectInfo->target));
		projectFile.Write();
		projectFile.Close();
		SetProjectState(false);
	}
}

void MainWindow::SetProjectState(const bool& changed) {
	if (m_projectInfo) {
		if (changed) {
			m_projectInfo->ProjectHaveChanges = true;
			SetLabel("[" + wxFileNameFromPath(m_projectInfo->filepath) + "*] - CodeEditor");
		}
		else {
			m_projectInfo->ProjectHaveChanges = false;
			SetLabel("[" + wxFileNameFromPath(m_projectInfo->filepath) + "] - CodeEditor");
		}
	}
	else {
		SetLabel("CodeEditor");
	}
}

void MainWindow::OnProjectOpen(wxCommandEvent& event) {
	wxFileDialog openProjectDialog(this, _("Open project"),
		wxEmptyString, wxEmptyString,
		_("Project file (*.pro)|*.pro"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openProjectDialog.ShowModal() == wxID_CANCEL) { // если нажата "отмена" в окне выбора файла
		return;
	}

	if (m_projectInfo) {
		OnProjectClose(event);
	}
	else {
		m_projectInfo = new ProjectInfo();
	}

	m_projectInfo->directory = openProjectDialog.GetDirectory();
	m_projectInfo->filepath = openProjectDialog.GetPath();

	LoadProject();
	if (m_projectInfo->execFile.IsEmpty() ||
		m_projectInfo->files.empty() ||
		m_projectInfo->mainFile.IsEmpty()) {
		wxMessageBox(_("Invalid opening project."),
			_("Opening project abort"),
			wxOK);
		OnProjectClose(event);
		return;
	}
	SetProjectState(false);
	m_ProjectExplorer->ConstructTree(GetProjectInfo());
}

void MainWindow::OnProjectClose(wxCommandEvent& event) {
	wxAuiNotebookEvent closePage(wxEVT_AUINOTEBOOK_PAGE_CLOSE, ID_MainWindow);
	while (m_Notebook->GetPageCount() > 0) {
		m_Notebook->ProcessWindowEvent(closePage);
		if (!closePage.IsAllowed()) {
			return;
		}
	}
	if (m_projectInfo && m_projectInfo->ProjectHaveChanges) {
		int response = wxMessageBox(_("Do you want to save the changes in this project?"),
			_("Confirm the action"),
			wxYES_NO | wxCANCEL,
			this);
		if (response == wxYES) {
			OnProjectSave(event);
		}
		else if (response == wxCANCEL) {
			return;
		}
	}
	delete m_projectInfo;
	m_projectInfo = nullptr;
	SetProjectState();
	m_ProjectExplorer->DeleteAllItems();
}

void MainWindow::LoadProject() {
	wxTextFile projectFile(m_projectInfo->filepath);
	if (projectFile.Open()) {
		for (wxString line = projectFile.GetFirstLine(); !projectFile.Eof(); line = projectFile.GetNextLine()) {
			if (line.IsEmpty() || line.StartsWith(';') || line.StartsWith('#')) {
				continue;
			}
			wxString key = line.Before('=');
			wxString value = line.After('=');
			value.Trim(false);
			if (value.StartsWith('\'')) {
				value = value.AfterFirst('\'');
				value = value.BeforeLast('\'');
			}
			else if (value.StartsWith('\"')) {
				value = value.AfterFirst('\"');
				value = value.BeforeLast('\"');
			}
			if ("IMPORT" == key) {
				m_projectInfo->imports.push_back(value);
			}
			else if ("FILE" == key) {
				m_projectInfo->files.push_back(value);
			}
			else if ("MAIN" == key) {
				m_projectInfo->mainFile = value;
			}
			else if ("EXEC" == key) {
				m_projectInfo->execFile = value;
			}
			else if ("TARGET" == key) {
				unsigned int i;
				value.ToUInt(&i);
				if (i < Targets.size()) {
					m_projectInfo->target = i;
				}
			}
		}
		projectFile.Close();
	}
}

void MainWindow::OnCreateFile(wxCommandEvent& event) {
	wxFileDialog createFileDialog(this, "New file",
		wxEmptyString, wxEmptyString,
		"O2M file (*.o)|*.o",
		wxFD_SAVE);
	if (createFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	if (m_projectInfo->directory != createFileDialog.GetDirectory()) {
		wxMessageBox(_("File cannot be created. "
			"File directory is different from the project directory"),
			_("Creating file abort"),
			wxOK, this);
	}
	else {
		wxString filepath = createFileDialog.GetPath();
		if (!wxFileExists(filepath)) {
			if (CreateNewFile(filepath)) {
				IncludeFile(wxFileNameFromPath(filepath));
			}
		}
		else {
			wxMessageBox(_("The selected file already exist. "
				"You can include it in the project by \"Include\""),
				_("Creating file abort"),
				wxOK, this);
		}
	}
}

void MainWindow::OnOpenFile(wxCommandEvent& event) {
	int index = event.GetInt();
	wxString filepath = m_projectInfo->directory + wxFileName::GetPathSeparator() + m_projectInfo->files[index];
	if (!wxFileExists(filepath)) {
		wxMessageBox(_("File does not exist."),
			_("Opening file abort"),
			wxOK, this);
	}
	else {
		event.SetString(filepath);
		m_Notebook->ProcessWindowEvent(event);
	}
}

void MainWindow::OnSetFileAsMain(wxCommandEvent& event) {
	int index = event.GetInt();

	m_projectInfo->mainFile = m_projectInfo->files[index];

	SetProjectState(true);
	m_ProjectExplorer->DeleteAllItems();
	m_ProjectExplorer->ConstructTree(GetProjectInfo());
}

void MainWindow::OnFileUp(wxCommandEvent& event) {
	int index = event.GetInt();

	if (index > 0) {
		wxString temp = m_projectInfo->files[index];
		m_projectInfo->files[index] = m_projectInfo->files[index - 1];
		m_projectInfo->files[index - 1] = temp;
	}

	SetProjectState(true);
	m_ProjectExplorer->DeleteAllItems();
	m_ProjectExplorer->ConstructTree(GetProjectInfo());
}

void MainWindow::OnFileDown(wxCommandEvent& event) {
	int index = event.GetInt();

	if (index < m_projectInfo->files.size() - 1) {
		wxString temp = m_projectInfo->files[index];
		m_projectInfo->files[index] = m_projectInfo->files[index + 1];
		m_projectInfo->files[index + 1] = temp;
	}

	SetProjectState(true);
	m_ProjectExplorer->DeleteAllItems();
	m_ProjectExplorer->ConstructTree(GetProjectInfo());
}

void MainWindow::OnExcludeFile(wxCommandEvent& event) {
	int index = event.GetInt();

	int response = wxMessageBox(_("Do you want to exclude this file from project?"),
		_("Confirm the action"),
		wxYES_NO, this);
	if (response == wxYES) {
		wxAuiNotebookEvent closePage(wxEVT_AUINOTEBOOK_PAGE_CLOSE, ID_MainWindow);
		wxString filename = m_projectInfo->files[index];
		closePage.SetString(m_projectInfo->directory + wxFileName::GetPathSeparator() + filename);
		m_Notebook->ProcessWindowEvent(closePage);
		if (!closePage.IsAllowed()) {
			return;
		}
		ExcludeFile(index, filename);

		SetProjectState(true);
		m_ProjectExplorer->DeleteAllItems();
		m_ProjectExplorer->ConstructTree(GetProjectInfo());
	}
}

void MainWindow::OnIncludeFile(wxCommandEvent& event) {
	wxFileDialog addFileDialog(this, "Include file",
		wxEmptyString, wxEmptyString,
		"O2M file (*.o)|*.o",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (addFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	if (m_projectInfo->directory != addFileDialog.GetDirectory()) {
		wxMessageBox(_("File cannot be included in the project. "
			"File directory is different from the project directory."),
			_("Including file abort"),
			wxOK, this);
	}
	else {
		wxString filename = addFileDialog.GetFilename();
		if (m_projectInfo->HasFile(filename)) {
			wxMessageBox(_("File cannot be included in the project. "
				"File is already included in the project."),
				_("Including file abort"),
				wxOK, this);
		}
		else {
			IncludeFile(filename);
		}
	}
}

void MainWindow::OnDeleteFile(wxCommandEvent& event) {
	int index = event.GetInt();

	int response = wxMessageBox(_("Do you want to delete this file permanently?"),
		_("Confirm the action"),
		wxYES_NO, this);
	if (response == wxYES) {
		wxAuiNotebookEvent closePage(wxEVT_AUINOTEBOOK_PAGE_CLOSE, ID_MainWindow);
		wxString filename = m_projectInfo->files[index];
		wxString filepath = m_projectInfo->directory + wxFileName::GetPathSeparator() + filename;
		closePage.SetString(filepath);
		m_Notebook->ProcessWindowEvent(closePage);
		if (!closePage.IsAllowed()) {
			return;
		}
		ExcludeFile(index, filename);
		if (wxFileExists(filepath)) {
			wxRemoveFile(filepath);
		}

		SetProjectState(true);
		m_ProjectExplorer->DeleteAllItems();
		m_ProjectExplorer->ConstructTree(GetProjectInfo());
	}
}

void MainWindow::OnExit(wxCommandEvent& WXUNUSED(event)) {
	Close();
}

void MainWindow::OnCloseWindow(wxCloseEvent& event) {
	wxCommandEvent closeProject(wxEVT_MENU);
	OnProjectClose(closeProject);
	if (m_projectInfo) {
		if (event.CanVeto()) {
			event.Veto();
			return;
		}
		else {
			delete m_projectInfo;
		}
	}
	this->Destroy();
}

void MainWindow::OnSetTargetPlatform(wxCommandEvent& event) {
	m_projectInfo->target = event.GetId() - ID_TargetMIN;
	SetProjectState(true);
}

void MainWindow::OnUpdateFileMenu(wxUpdateUIEvent& event) {
	switch (event.GetId())
	{
	case ID_New:
	case ID_IncludeFile:
		event.Enable(m_projectInfo);
		break;
	case ID_Save:
		event.Enable(m_Notebook->GetPageCount() > 0);
		break;
	}
}

void MainWindow::OnUpdateProjectMenu(wxUpdateUIEvent& event) {
	switch (event.GetId())
	{
	case ID_ProjectNew:
	case ID_ProjectOpen:
		event.Enable(!m_projectInfo);
		break;
	case ID_ProjectSave:
		event.Enable(m_projectInfo && m_projectInfo->ProjectHaveChanges);
		break;
	case ID_ProjectClose:
		event.Enable(m_projectInfo);
	}
}

void MainWindow::OnUpdateHighlightMenu(wxUpdateUIEvent& event) {
	if (m_Notebook->GetPageCount() > 0) {
		event.Enable(true);
		event.Check(m_Notebook->IsIndexOfCurrentHighlightLanguage(event.GetId() - ID_HIGHLIGHTFIRST));
	}
	else {
		event.Enable(false);
	}
}

void MainWindow::OnUpdateTargetPlatformMenu(wxUpdateUIEvent& event) {
	if (m_projectInfo) {
		event.Enable(true);
		event.Check(event.GetText() == Targets[event.GetId() - ID_TargetMIN]);
	}
	else {
		event.Enable(false);
	}
}

wxMenuBar* MainWindow::CreateMenuBar() {
	wxMenuBar *menuBar = new wxMenuBar();
	wxMenu *fileMenu = new wxMenu();
	fileMenu->Append(ID_New, _("&New"));
	fileMenu->Append(ID_Save, _("&Save"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_IncludeFile, _("&Include"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT);

	wxMenu* projectMenu = new wxMenu();
	projectMenu->Append(ID_ProjectNew, _("&New project"));
	projectMenu->Append(ID_ProjectOpen, _("&Open project"));
	projectMenu->Append(ID_ProjectSave, _("&Save project"));
	projectMenu->AppendSeparator();
	projectMenu->Append(ID_ProjectClose, _("&Close project"));

	wxMenu* targetPlatformMenu = new wxMenu();
	for (int i = 0; i < Targets.size(); i++) {
		targetPlatformMenu->AppendRadioItem(ID_TargetMIN + i, Targets[i]);
	}

	wxMenu* settingsMenu = new wxMenu();
	settingsMenu->Append(wxID_ANY, _("&Target platform"), targetPlatformMenu);

	wxMenu* runMenu = new wxMenu();
	runMenu->Append(wxID_EXECUTE);

	wxMenu* syntaxHighlightMenu = new wxMenu();
	for (int i = 0; i < g_LanguageSettings.size(); i++) {
		syntaxHighlightMenu->AppendRadioItem(ID_HIGHLIGHTFIRST + i, g_LanguageSettings[i].name);
	}

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(projectMenu, _("&Project"));
	menuBar->Append(settingsMenu, _("&Settings"));
	menuBar->Append(runMenu, _("&Run"));
	menuBar->Append(syntaxHighlightMenu, _("&Highlight language"));

	return menuBar;
}

Notebook* MainWindow::CreateNotebook() {
	// create the notebook off-window to avoid flicker
	wxSize client_size = GetClientSize();

	Notebook* ctrl = new Notebook(this, ID_Notebook, 
		wxDefaultPosition, wxDefaultSize, m_NotebookStyle);
	ctrl->Freeze();
	ctrl->SetArtProvider(new wxAuiSimpleTabArt);
	ctrl->Thaw();
	return ctrl;
}

ProjectExplorer* MainWindow::CreateProjectExplorer() {
	ProjectExplorer* projectExplorer = new ProjectExplorer(this, ID_ProjectExplorer,
		wxPoint(0, 0),
		FromDIP(wxSize(160, 250)),
		wxTR_DEFAULT_STYLE | wxNO_BORDER);

	return projectExplorer;
}

bool MainWindow::CreateNewFile(const wxString& filepath) {
	wxTextFile file;

	if (file.Create(filepath)) {
		file.Close();
		return true;
	}
	return false;
}

const ProjectInfo MainWindow::GetProjectInfo() {
	return *m_projectInfo;
}

void MainWindow::IncludeFile(const wxString& filename) {
	m_projectInfo->files.push_back(filename);

	m_ProjectExplorer->DeleteAllItems();
	m_ProjectExplorer->ConstructTree(GetProjectInfo());
	SetProjectState(true);
}

void MainWindow::ExcludeFile(const int& index, const wxString& filename) {
	m_projectInfo->files.erase(m_projectInfo->files.begin() + index);
	if (filename == m_projectInfo->mainFile) {
		m_projectInfo->mainFile =
			m_projectInfo->files.size() > 0 ?
			m_projectInfo->files[0] : "";
	}
}