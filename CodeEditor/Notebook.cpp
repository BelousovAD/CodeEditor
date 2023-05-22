#include "Notebook.h"

#include "wx/msgdlg.h"
#include "wx/filedlg.h"

Notebook::Notebook(wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style)
    : wxAuiNotebook(parent, id, pos, size, style) {
	Bind(wxEVT_MENU, &Notebook::OnFileOpen, this, ID_Open);
	Bind(wxEVT_STC_SAVEPOINTREACHED, &Notebook::OnSavePointReached, this, ID_Editor);
	Bind(wxEVT_STC_SAVEPOINTLEFT, &Notebook::OnSavePointLeft, this, ID_Editor);
	Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &Notebook::OnNotebookPageCloseByNotebook, this, ID_Notebook);
	Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &Notebook::OnNotebookPageCloseByMainWindow, this, ID_MainWindow);
}

Notebook::~Notebook() {
	while (GetPageCount() > 0) {
		GetPage(0)->Destroy();
	}
}

void Notebook::OnFileOpen(wxCommandEvent& event) {
	wxString filepath = event.GetString();

	int index = IndexIfFileOpenedInNotebook(filepath);
	if (index != -1) {
		SetSelection(index); // сделать вкладку активной
		return;
	}

	Freeze(); // заморозка блокнота
	Editor* newPage = CreateEditor(); // создать вкладку
	AddPage(newPage, wxFileNameFromPath(filepath), true); // добавить вкладку в блокнот
	newPage->ProcessWindowEvent(event);
	Thaw(); // разморозка блокнота
}

void Notebook::OnNotebookPageCloseByNotebook(wxAuiNotebookEvent& event) {
	Editor* currentPage = (Editor*)GetCurrentPage(); // указатель на текущую вкладку
	if (currentPage->IsModified()) {
		int response = wxMessageBox(_("Do you want to save the changes in this file?"),
			_("Confirm the action"),
			wxYES_NO | wxCANCEL,
			this);
		if (response == wxYES) {
			currentPage->SaveFile();
		}
		else {
			if (response == wxCANCEL) {
				event.Veto();
				return;
			}
		}
	}
}

void Notebook::OnNotebookPageCloseByMainWindow(wxAuiNotebookEvent& event) {
	Editor* currentPage = nullptr;
	wxString filepath = event.GetString();
	int	index = filepath.IsEmpty() ? 
		0 : IndexIfFileOpenedInNotebook(filepath);
	if (index == -1) {
		return;
	}
	currentPage = (Editor*)GetPage(index);
	if (currentPage->IsModified()) {
		int response = wxMessageBox(
			wxString::Format(_("Do you want to save the changes in file %s?"),
				GetPageText(index).Before('*')),
			_("Confirm the action"),
			wxYES_NO | wxCANCEL,
			this);
		if (response == wxYES) {
			currentPage->SaveFile();
		}
		else {
			if (response == wxCANCEL) {
				event.Veto();
				return;
			}
		}
	}
	this->DeletePage(index);
}

void Notebook::OnSavePointReached(wxStyledTextEvent& WXUNUSED(event)) {
	int indexCurrentPage = GetSelection(); // индекс текущей вкладки
	if (indexCurrentPage == -1) {
		return;
	}

	Editor* currentPage = (Editor*)GetCurrentPage(); // указатель на текущую вкладку
	SetPageText(indexCurrentPage,
		currentPage->IsFilepathEmpty() ?
		GetPageText(indexCurrentPage).Before('*') :
		wxFileNameFromPath(currentPage->GetFilepath())); // изменить название вкладки
}

void Notebook::OnSavePointLeft(wxStyledTextEvent& WXUNUSED(event)) {
	int indexCurrentPage = GetSelection(); // индекс текущей вкладки
	if (indexCurrentPage == -1) {
		return;
	}

	Editor* currentPage = (Editor*)GetCurrentPage(); // указатель на текущую вкладку
	SetPageText(indexCurrentPage, GetPageText(indexCurrentPage) + '*'); // изменить название вкладки
}

void Notebook::OnSaveFile(wxCommandEvent& event) {
	wxWindow* currentPage = GetCurrentPage();
	if (currentPage != nullptr) {
		currentPage->ProcessWindowEvent(event);
	}
}

void Notebook::OnHighlightLanguage(wxCommandEvent& event) {
	wxWindow* currentPage = GetCurrentPage();
	if (currentPage != nullptr) {
		currentPage->ProcessWindowEvent(event);
	}
}

const bool Notebook::IsIndexOfCurrentHighlightLanguage(const size_t& index) {
	Editor* currentPage = (Editor*)GetCurrentPage();
	if (currentPage != nullptr) {
		return currentPage->GetLanguageInfo().name == g_LanguageSettings[index].name;
	}
	return false;
}

int Notebook::IndexIfFileOpenedInNotebook(const wxString& filepath) {
	for (size_t i = 0; i < GetPageCount(); i++) { // цикл по вкладкам
		if (((Editor*)GetPage(i))->GetFilepath() == filepath) { // если выбранный файл уже открыт
			return i;
		}
	}
	return -1;
}

Editor* Notebook::CreateEditor() {
	Editor* editor = new Editor(this, ID_Editor);
	return editor;
}
