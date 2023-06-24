#include "MainWindow.h"
#include "defsexternal.h"

#include "HelpWindow.h"
#include "ObjectExplorerWindow.h"
#include "wx/dir.h"
#include <shellapi.h>

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
	m_ImportsList = CreateImportsList();
	m_Console = CreateConsole();
	
	SetMenuBar(CreateMenuBar());

	m_AuiManager->AddPane(m_Notebook, wxAuiPaneInfo().
		Name("Notebook").
		CenterPane());
	m_AuiManager->AddPane(m_ProjectExplorer, wxAuiPaneInfo().
		Name("ProjectExplorer").Caption(_("Project explorer")).
		Layer(1).Left().Row(0).Position(0).
		CloseButton(false).MaximizeButton(true));
	m_AuiManager->AddPane(m_ImportsList, wxAuiPaneInfo().
		Name("ImportsList").Caption(_("Imports list")).
		Layer(1).Left().Row(0).Position(1).
		CloseButton(false).MaximizeButton(true));
	m_AuiManager->AddPane(m_Console, wxAuiPaneInfo().
		Name("Console").Caption(_("Console")).
		Layer(0).Bottom().Row(0).Position(0).
		CloseButton(false).MaximizeButton(true));

	SetStatusBar(new wxStatusBar(this));

	m_AuiManager->Update();

	Bind(wxEVT_MENU, &MainWindow::OnDirAppend, this, ID_DirAppend);
	Bind(wxEVT_MENU, &MainWindow::OnDirUp, this, ID_DirUp);
	Bind(wxEVT_MENU, &MainWindow::OnDirDown, this, ID_DirDown);
	Bind(wxEVT_MENU, &MainWindow::OnDirDelete, this, ID_DirDelete);

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

	Bind(wxEVT_MENU, &MainWindow::OnObjectExplorerOpen, this, ID_ObjectExplorerOpen);

	Bind(wxEVT_MENU, &MainWindow::OnCompile, this, ID_Compile);
	Bind(wxEVT_MENU, &MainWindow::OnBuild, this, ID_Build);
	Bind(wxEVT_MENU, &MainWindow::OnExecute, this, ID_Execute);

	Bind(wxEVT_MENU, &Notebook::OnHighlightLanguage, m_Notebook, ID_HIGHLIGHTFIRST, ID_HIGHLIGHTLAST);
	Bind(wxEVT_MENU, &MainWindow::OnSetTargetPlatform, this, ID_TargetMIN, ID_TargetMAX);

	Bind(wxEVT_MENU, &MainWindow::OnHelp, this, ID_Help);

	Bind(wxEVT_CLOSE_WINDOW, &MainWindow::OnCloseWindow, this);

	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateFileMenu, this, ID_New, ID_IncludeFile);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateProjectMenu, this, ID_ProjectNew, ID_ProjectClose);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateRunMenu, this, ID_Compile, ID_Execute);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateTargetPlatformMenu, this, ID_TargetMIN, ID_TargetMAX);
	Bind(wxEVT_UPDATE_UI, &MainWindow::OnUpdateHighlightMenu, this, ID_HIGHLIGHTFIRST, ID_HIGHLIGHTLAST);

	if (!m_config.LoadConfig()) {
		m_config.SaveConfig();
		wxMessageBox(_("Please fill in the \"CodeEditor.ini\" file."),
			_("Starting abort"),
			wxOK, this);
		Close();
	}
	else {
		m_ImportsList->ConstructList(m_config.imports);
	}
}

MainWindow::~MainWindow() {
	m_Notebook->Destroy();
	m_ProjectExplorer->Destroy();
	m_ImportsList->Destroy();
	m_Console->Destroy();
	m_config.SaveConfig();
	delete m_AuiManager;
}

void MainWindow::OnProjectNew(wxCommandEvent& event) {
	if (m_projectInfo) {
		wxMessageBox(_("Cannot create the project. "
			"You are required to close the currently open project."),
			_("Error"),
			wxOK, this);
		return;
	}
	else {
		m_projectInfo = new ProjectInfo();
	}

	wxDirDialog newProjectDialog(this, _("Choose directory"),
		wxEmptyString,
		wxDD_DEFAULT_STYLE | wxDD_CHANGE_DIR);
	if (newProjectDialog.ShowModal() == wxID_CANCEL) {
		return;
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
	m_projectInfo->SaveProjectInfo();
	DeleteTempDirectories();
	SetProjectState(false);
}

void MainWindow::OnProjectOpen(wxCommandEvent& event) {
	if (m_projectInfo) {
		wxMessageBox(_("Cannot create the project. "
			"You are required to close the currently open project."),
			_("Error"),
			wxOK, this);
		return;
	}
	else {
		m_projectInfo = new ProjectInfo();
	}

	wxFileDialog openProjectDialog(this, _("Open project"),
		wxEmptyString, wxEmptyString,
		_("Project file (*.pro)|*.pro"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openProjectDialog.ShowModal() == wxID_CANCEL) { // если нажата "отмена" в окне выбора файла
		return;
	}

	m_projectInfo->directory = openProjectDialog.GetDirectory();
	m_projectInfo->filepath = openProjectDialog.GetPath();

	m_projectInfo->LoadProjectInfo();
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

void MainWindow::OnObjectExplorerOpen(wxCommandEvent& event) {
	ObjectExplorerWindow ObjectExplorer(this, GetProjectInfo());
	ObjectExplorer.ShowModal();
}

void MainWindow::OnDirAppend(wxCommandEvent& event) {
	if (!m_ImportsList) {
		return;
	}
	wxDirDialog appendDirDialog(this, _("Include directory"),
		wxEmptyString,
		wxDD_DEFAULT_STYLE);

	if (appendDirDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	wxString directory = appendDirDialog.GetPath() + wxFileName::GetPathSeparator();

	if (std::find(m_config.imports.begin(),
			m_config.imports.end(),
			directory) != m_config.imports.end()) {
		wxMessageBox(_("Directory is already in the list."),
			_("Appending directory abort"),
			wxOK, this);
	}
	else {
		m_config.imports.push_back(directory);
		m_ImportsList->DeleteAllItems();
		m_ImportsList->DeleteAllColumns();
		m_ImportsList->ConstructList(m_config.imports);
	}
}

void MainWindow::OnDirUp(wxCommandEvent& event) {
	if (!m_ImportsList) {
		return;
	}
	int index = event.GetInt();

	if (index > 0) {
		wxString temp = m_config.imports[index];
		m_config.imports[index] = m_config.imports[index - 1];
		m_config.imports[index - 1] = temp;
	}

	m_ImportsList->DeleteAllItems();
	m_ImportsList->DeleteAllColumns();
	m_ImportsList->ConstructList(m_config.imports);
}

void MainWindow::OnDirDown(wxCommandEvent& event) {
	if (!m_ImportsList) {
		return;
	}
	int index = event.GetInt();

	if (index < m_config.imports.size() - 1) {
		wxString temp = m_config.imports[index];
		m_config.imports[index] = m_config.imports[index + 1];
		m_config.imports[index + 1] = temp;
	}

	m_ImportsList->DeleteAllItems();
	m_ImportsList->DeleteAllColumns();
	m_ImportsList->ConstructList(m_config.imports);
}

void MainWindow::OnDirDelete(wxCommandEvent& event) {
	if (!m_ImportsList) {
		return;
	}
	int index = event.GetInt();

	m_config.imports.erase(m_config.imports.begin() + index);

	m_ImportsList->DeleteAllItems();
	m_ImportsList->DeleteAllColumns();
	m_ImportsList->ConstructList(m_config.imports);
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

void MainWindow::OnCompile(wxCommandEvent& event) {
	if (!m_projectInfo) {
		wxMessageBox(_("Need to open a project."),
			_("Compiling abort"),
			wxOK, this);
		return;
	}
	else if (m_projectInfo->files.empty()) {
		wxMessageBox(_("There are no files in the project."),
			_("Compiling abort"),
			wxOK, this);
		return;
	}
	
	CreateTempDirectories();
	ClearTempDirectories();

	m_Console->Clear();

	wxDir projectDir(m_projectInfo->directory);
	CreateNewFile(projectDir.GetNameWithSep() + "CompileInfo.tmp");
	SaveCompileInfo(projectDir.GetNameWithSep() + "CompileInfo.tmp");

	for (size_t i = 0; i < m_projectInfo->files.size(); i++) {
		if (!SyncExecWithCaptureOutput('"' + m_config.O2MPath + "\" \"" +
			m_projectInfo->files[i] + "\" " +
			"CompileInfo.tmp" + " -a", projectDir.GetName(), wxFileNameFromPath(m_config.O2MPath))) {
			return;
		}
	}

	if (wxFileExists(projectDir.GetNameWithSep() + "CompileInfo.tmp")) {
		wxRemoveFile(projectDir.GetNameWithSep() + "CompileInfo.tmp");
	}

	wxString filepath = wxString("CPP") +
		wxFileName::GetPathSeparator() +
		"_O2M_make.2mk";
	if (wxFileExists(projectDir.GetNameWithSep() +
		filepath)) {
		if (!SyncExecWithCaptureOutput('"' + m_config.Link2MPath + "\" \"" +
			filepath + "\" -a", projectDir.GetName(), wxFileNameFromPath(m_config.Link2MPath))) {
			return;
		}
	}
	else {
		return;
	}

	wxString st = "p 0";
	switch (m_projectInfo->target) {
	case 1: 
		st = "p 1";
		break;
	case 2:
		if (m_config.WinToOEM) {
			st = "p 2 -c";
		}
		else {
			st = "p 2";
		}
		break;
	case 3:
		if (m_config.WinToOEM) {
			st = "p 3 -c";
		}
		else {
			st = "p 3";
		}
		break;
	}

	if (wxFileExists(projectDir.GetNameWithSep() +
		filepath)) {
		if (!SyncExecWithCaptureOutput('"' + m_config.Make2MPath + "\" \"" +
			filepath + "\" \"" +
			"CPP" + wxFileName::GetPathSeparator() + m_config.Makefile + "\" -a -" + st +
			" -cp \"" + m_config.CppPath + '"', projectDir.GetName(), wxFileNameFromPath(m_config.Make2MPath))) {
			return;
		}
	}
	else {
		return;
	}
}

void MainWindow::OnBuild(wxCommandEvent& event) {
	if (!m_projectInfo) {
		wxMessageBox(_("Need to open a project."),
			_("Building abort"),
			wxOK, this);
		return;
	}

	m_Console->Clear();

	wxDir dir(m_projectInfo->directory +
		wxFileName::GetPathSeparator() +
		"Make");
	if (dir.IsOpened()) {
		wxString filename;
		bool cont = dir.GetFirst(&filename);
		while (cont) {
			wxRemoveFile(dir.GetNameWithSep() + filename);
			cont = dir.GetNext(&filename);
		}
	}
	
	dir.Close();
	dir.Open(m_projectInfo->directory +
		wxFileName::GetPathSeparator() +
		"CPP");

	if (wxFileExists(dir.GetNameWithSep() + m_config.Makefile)) {
		if (2 <= m_projectInfo->target) {
			SyncExecWithCaptureOutput('"' + m_config.MakeCmdLine + "\" -f \"" +
				m_config.Makefile + '"', dir.GetName(), "MinGW");
		}
		else {
			SyncExecWithCaptureOutput(wxString("VsDevCmd.bat && ") +
				"\"" + m_config.NMakeCmdLine + "\" -f \"" + m_config.Makefile + '"', dir.GetName(), "NMAKE");
		}
	}
	else {
		wxMessageBox(_("Need to compile a project."),
			_("Building abort"),
			wxOK, this);
		return;
	}
}

void MainWindow::OnExecute(wxCommandEvent& event) {
	if (!m_projectInfo) {
		wxMessageBox(_("Need to open a project."),
			_("Executing abort"),
			wxOK, this);
		return;
	}

	wxDir dir(m_projectInfo->directory +
		wxFileName::GetPathSeparator() +
		"Make");

	if (wxFileExists(dir.GetNameWithSep() + m_projectInfo->execFile)) {
		wxString batFilePath = dir.GetNameWithSep() + m_projectInfo->execFile + "\.bat";
		if (wxFileExists(batFilePath)) {
			wxRemoveFile(batFilePath);
		}
		CreateNewFile(batFilePath);
		wxTextFile file(batFilePath);
		if (file.Open()) {
			file.AddLine('@' + m_projectInfo->execFile);
			file.AddLine("@echo.");
			file.AddLine("@PAUSE");
			file.Write();
			file.Close();
		}
		else {
			wxMessageBox(_("Cannot create \"\.bat\" file."),
				_("Executing abort"),
				wxOK, this);
			return;
		}
		AsyncExec(batFilePath, dir.GetName(), m_projectInfo->execFile);
	}
	else {
		wxMessageBox(_("Need to build a project."),
			_("Executing abort"),
			wxOK, this);
		return;
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

void MainWindow::OnHelp(wxCommandEvent& event) {
	HelpWindow Help(this);
	Help.ShowModal();
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
	switch (event.GetId()) {
	case ID_ProjectNew:
	case ID_ProjectOpen:
		event.Enable(!m_projectInfo);
		break;
	case ID_ProjectSave:
		event.Enable(m_projectInfo && m_projectInfo->ProjectHaveChanges);
		break;
	case ID_ProjectClose:
	case ID_ObjectExplorerOpen:
		event.Enable(m_projectInfo);
		break;
	}
}

void MainWindow::OnUpdateRunMenu(wxUpdateUIEvent& event) {
	switch (event.GetId()) {
	case ID_Execute:
		event.Enable(m_projectInfo &&
			!m_projectInfo->ProjectHaveChanges &&
			wxFileExists(m_projectInfo->directory +
				wxFileName::GetPathSeparator() +
				"Make" +
				wxFileName::GetPathSeparator() +
				m_projectInfo->execFile));
	case ID_Build:
		event.Enable(m_projectInfo &&
			!m_projectInfo->ProjectHaveChanges &&
			wxFileExists(m_projectInfo->directory +
				wxFileName::GetPathSeparator() +
				"CPP" +
				wxFileName::GetPathSeparator() +
				m_config.Makefile));
		break;
	case ID_Compile:
		event.Enable(m_projectInfo && !m_projectInfo->ProjectHaveChanges);
		break;
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

bool MainWindow::SyncExecWithCaptureOutput(const wxString command, const wxString cwd, const wxString appName) {
	wxLogStatus("\"%s\" is running please wait...", appName);

	wxArrayString output, errors;
	wxExecuteEnv* env = new wxExecuteEnv();
	env->cwd = cwd;
	int code = wxExecute(command, output, errors, wxEXEC_SYNC, env);

	wxLogStatus("\"%s\" terminated with exit code %d.",
		appName, code);

	m_Console->ShowOutput(command, output, "Output", appName);
	m_Console->ShowOutput(command, errors, "Errors", appName);
	return code == 0;
}

bool MainWindow::AsyncExec(const wxString batFilePath, const wxString cwd, const wxString appName) {
	wxLogStatus("\"%s\" is running please wait...", appName);

	wxExecuteEnv* env = new wxExecuteEnv();
	env->cwd = cwd;
	int code = (int)ShellExecuteW(NULL, NULL, batFilePath, NULL, cwd, SW_SHOWNORMAL);

	wxLogStatus("\"%s\" terminated with exit code %d.",
		appName, code);
	return code > 32;
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

void MainWindow::SaveCompileInfo(const wxString& filepath) {
	wxTextFile projectFile(filepath);
	if (projectFile.Open()) {
		projectFile.Clear();
		for (int i = 0; i < m_config.imports.size(); i++) {
			projectFile.AddLine("IMPORT=" + m_config.imports[i]);
		}
		projectFile.AddLine("MAIN=" + m_projectInfo->mainFile);
		projectFile.AddLine("EXEC=" + m_projectInfo->execFile);
		projectFile.AddLine(wxString::Format("TABSIZE=%d", m_config.tabSize));
		projectFile.Write();
		projectFile.Close();
	}
}

ImportsList* MainWindow::CreateImportsList() {
	return new ImportsList(this);
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
	projectMenu->Append(ID_ObjectExplorerOpen, _("&Open Object explorer"));
	projectMenu->AppendSeparator();
	projectMenu->Append(ID_ProjectClose, _("&Close project"));

	wxMenu* targetPlatformMenu = new wxMenu();
	for (int i = 0; i < Targets.size(); i++) {
		targetPlatformMenu->AppendRadioItem(ID_TargetMIN + i, Targets[i]);
	}

	wxMenu* settingsMenu = new wxMenu();
	settingsMenu->Append(wxID_ANY, _("&Target platform"), targetPlatformMenu);
	settingsMenu->AppendSeparator();
	settingsMenu->Append(ID_DirAppend, _("&Add import directory"));

	wxMenu* runMenu = new wxMenu();
	runMenu->Append(ID_Compile, _("&Compile"));
	runMenu->Append(ID_Build, _("&Build"));
	runMenu->Append(ID_Execute, _("&Run"));

	wxMenu* syntaxHighlightMenu = new wxMenu();
	for (int i = 0; i < g_LanguageSettings.size(); i++) {
		syntaxHighlightMenu->AppendRadioItem(ID_HIGHLIGHTFIRST + i, g_LanguageSettings[i].name);
	}

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(ID_Help, _("&Help"));

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(projectMenu, _("&Project"));
	menuBar->Append(settingsMenu, _("&Settings"));
	menuBar->Append(runMenu, _("&Run"));
	menuBar->Append(syntaxHighlightMenu, _("&Highlight language"));
	menuBar->Append(helpMenu, _("&Help"));

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
	return new ProjectExplorer(this, ID_ProjectExplorer,
		wxPoint(0, 0),
		FromDIP(wxSize(160, 250)),
		wxTR_DEFAULT_STYLE | wxNO_BORDER);
}

Console* MainWindow::CreateConsole() {
	return new Console(this);
}

bool MainWindow::CreateNewFile(const wxString& filepath) {
	wxTextFile file;

	if (file.Create(filepath)) {
		file.Close();
		return true;
	}
	return false;
}

void MainWindow::CreateTempDirectories() {
	if (!wxDirExists(m_projectInfo->directory + 
		wxFileName::GetPathSeparator() + "CPP")) {
		wxDir::Make(m_projectInfo->directory +
			wxFileName::GetPathSeparator() + "CPP");
	}
	if (!wxDirExists(m_projectInfo->directory +
		wxFileName::GetPathSeparator() + "DFN")) {
		wxDir::Make(m_projectInfo->directory +
			wxFileName::GetPathSeparator() + "DFN");
	}
	if (!wxDirExists(m_projectInfo->directory +
		wxFileName::GetPathSeparator() + "Make")) {
		wxDir::Make(m_projectInfo->directory +
			wxFileName::GetPathSeparator() + "Make");
	}
}

void MainWindow::DeleteTempDirectories() {
	ClearTempDirectories();

	if (wxDirExists(m_projectInfo->directory +
		wxFileName::GetPathSeparator() + "CPP")) {
		wxDir::Remove(m_projectInfo->directory +
			wxFileName::GetPathSeparator() + "CPP");
	}
	if (wxDirExists(m_projectInfo->directory +
		wxFileName::GetPathSeparator() + "DFN")) {
		wxDir::Remove(m_projectInfo->directory +
			wxFileName::GetPathSeparator() + "DFN");
	}
	if (wxDirExists(m_projectInfo->directory +
		wxFileName::GetPathSeparator() + "Make")) {
		wxDir::Remove(m_projectInfo->directory +
			wxFileName::GetPathSeparator() + "Make");
	}
}

void MainWindow::ClearTempDirectories() {
	wxString dirs[] = { "CPP", "DFN", "Make" };
	for (int i = 0; i < 3; i++) {
		wxDir dir(m_projectInfo->directory +
			wxFileName::GetPathSeparator() +
			dirs[i]);
		if (dir.IsOpened()) {
			wxString filename;
			bool cont = dir.GetFirst(&filename);
			while (cont) {
				wxRemoveFile(dir.GetNameWithSep() + filename);
				cont = dir.GetNext(&filename);
			}
		}
	}
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