#pragma once
#include <wx/wxprec.h>

#include "wx/aui/aui.h"
#include "wx/aui/framemanager.h"
#include "wx/filename.h"
#include "wx/menu.h"
#include "wx/stc/stc.h"
#include "wx/treectrl.h"

#include "Notebook.h"
#include "ProjectExplorer.h"

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

	void OnCreateFile(wxCommandEvent& event);
	void OnOpenFile(wxCommandEvent& event);
	void OnSetFileAsMain(wxCommandEvent& event);
	void OnFileUp(wxCommandEvent& event);
	void OnFileDown(wxCommandEvent& event);
	void OnExcludeFile(wxCommandEvent& event);
	void OnIncludeFile(wxCommandEvent& event);
	void OnDeleteFile(wxCommandEvent& event);

	void OnExit(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent& event);

	void OnSetTargetPlatform(wxCommandEvent& event);

	void OnUpdateFileMenu(wxUpdateUIEvent& event);
	void OnUpdateProjectMenu(wxUpdateUIEvent& event);
	void OnUpdateHighlightMenu(wxUpdateUIEvent& event);
	void OnUpdateTargetPlatformMenu(wxUpdateUIEvent& event);

	void LoadProject();
	void SetProjectState(const bool& changed = true);

	wxMenuBar* CreateMenuBar();
	Notebook* CreateNotebook();
	ProjectExplorer* CreateProjectExplorer();
	bool CreateNewFile(const wxString& filepath);
	void IncludeFile(const wxString& filename);
	void ExcludeFile(const int& index, const wxString& filename);

	const ProjectInfo GetProjectInfo();

private:
	wxAuiManager* m_AuiManager = nullptr;
	Notebook* m_Notebook = nullptr;
	ProjectExplorer* m_ProjectExplorer = nullptr;

	long m_NotebookStyle = NULL;
	ProjectInfo* m_projectInfo = nullptr;
};