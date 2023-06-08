#pragma once
#include <wx/wxprec.h>

#include "wx/aui/aui.h"
#include "wx/aui/framemanager.h"
#include "wx/filename.h"
#include "wx/menu.h"
#include "wx/stc/stc.h"
#include "wx/treectrl.h"

#include "ImportsList.h"
#include "Notebook.h"
#include "ProjectExplorer.h"
#include "CodeEditorConfig.h"
#include "Console.h"

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif // WX_PRECOMP

class MainWindow : public wxFrame {
public:
	MainWindow(wxWindow* parent,
		wxWindowID id,
		const wxString& title = "CodeEditor",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxASCII_STR(wxFrameNameStr));
	~MainWindow();

private:
	void OnProjectNew(wxCommandEvent& event);
	void OnProjectSave(wxCommandEvent& event);
	void OnProjectOpen(wxCommandEvent& event);
	void OnProjectClose(wxCommandEvent& event);

	void OnDirAppend(wxCommandEvent& event);
	void OnDirUp(wxCommandEvent& event);
	void OnDirDown(wxCommandEvent& event);
	void OnDirDelete(wxCommandEvent& event);

	void OnCreateFile(wxCommandEvent& event);
	void OnOpenFile(wxCommandEvent& event);
	void OnSetFileAsMain(wxCommandEvent& event);
	void OnFileUp(wxCommandEvent& event);
	void OnFileDown(wxCommandEvent& event);
	void OnExcludeFile(wxCommandEvent& event);
	void OnIncludeFile(wxCommandEvent& event);
	void OnDeleteFile(wxCommandEvent& event);

	void OnCompile(wxCommandEvent& event);
	void OnBuild(wxCommandEvent& event);
	void OnExecute(wxCommandEvent& event);

	void OnExit(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent& event);

	void OnSetTargetPlatform(wxCommandEvent& event);

	void OnUpdateFileMenu(wxUpdateUIEvent& event);
	void OnUpdateProjectMenu(wxUpdateUIEvent& event);
	void OnUpdateRunMenu(wxUpdateUIEvent& event);
	void OnUpdateHighlightMenu(wxUpdateUIEvent& event);
	void OnUpdateTargetPlatformMenu(wxUpdateUIEvent& event);

private:
	bool SyncExecWithCaptureOutput(const wxString command, const wxString cwd, const wxString appName);
	bool AsyncExec(const wxString command, const wxString cwd, const wxString appName);
	void SetProjectState(const bool& changed = true);
	void SaveCompileInfo(const wxString& filepath);

	ImportsList* CreateImportsList();
	wxMenuBar* CreateMenuBar();
	Notebook* CreateNotebook();
	ProjectExplorer* CreateProjectExplorer();
	Console* CreateConsole();
	bool CreateNewFile(const wxString& filepath);
	void CreateTempDirectories();
	void ClearTempDirectories();
	void IncludeFile(const wxString& filename);
	void ExcludeFile(const int& index, const wxString& filename);

	const ProjectInfo GetProjectInfo();

private:
	wxAuiManager* m_AuiManager = nullptr;
	ImportsList* m_ImportsList = nullptr;
	Notebook* m_Notebook = nullptr;
	Console* m_Console = nullptr;
	ProjectExplorer* m_ProjectExplorer = nullptr;

	long m_NotebookStyle = NULL;
	ProjectInfo* m_projectInfo = nullptr;
	CodeEditorConfig m_config;
};