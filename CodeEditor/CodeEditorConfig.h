#pragma once
#include <vector>
#include "wx/string.h"
#include "wx/textfile.h"

struct CodeEditorConfig {
	wxString filepath = "CodeEditor.ini";
	std::vector<wxString> imports;
	unsigned int tabSize = 4;
	wxString O2MPath = "";
	wxString Link2MPath = "";
	wxString Make2MPath = "";
	wxString Makefile = "";
	wxString NMakeCmdLine = "";
	wxString CppPath = "";
	wxString MakeCmdLine = "";
	int WinToOEM = 0;

	bool LoadConfig() {
		wxTextFile projectFile(filepath);
		if (!projectFile.Exists()) {
			return false;
		}		
		if (projectFile.Open()) {
			if (projectFile.Eof()) {
				return false;
			}
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
				if (value.IsEmpty()) {
					break;
				}
				else if ("IMPORT" == key) {
					imports.push_back(value);
				}
				else if ("TABSIZE" == key) {
					unsigned int i;
					value.ToUInt(&i);
					tabSize = i;
				}
				else if ("O2M" == key) {
					O2MPath = value;
				}
				else if ("LINK2M" == key) {
					Link2MPath = value;
				}
				else if ("MAKE2M" == key) {
					Make2MPath = value;
				}
				else if ("MAKEFILE" == key) {
					Makefile = value;
				}
				else if ("NMAKE" == key) {
					NMakeCmdLine = value;
				}
				else if ("CPP" == key) {
					CppPath = value;
				}
				else if ("MAKE" == key) {
					MakeCmdLine = value;
				}
				else if ("WINTOOEM" == key) {
					unsigned int i;
					value.ToUInt(&i);
					WinToOEM = i;
				}
				else {
					break;
				}
			}
			projectFile.Close();
			return true;
		}
		else {
			return false;
		}
	}

	bool SaveConfig() {
		wxTextFile projectFile;
		if (projectFile.Create(filepath)) {
			projectFile.Close();
		}
		if (projectFile.Open(filepath)) {
			projectFile.Clear();
			for (int i = 0; i < imports.size(); i++) {
				projectFile.AddLine("IMPORT=" + imports[i]);
			}
			projectFile.AddLine(wxString::Format("TABSIZE=%d", tabSize));
			projectFile.AddLine("O2M=" + O2MPath);
			projectFile.AddLine("LINK2M=" + Link2MPath);
			projectFile.AddLine("MAKE2M=" + Make2MPath);
			projectFile.AddLine("MAKEFILE=" + Makefile);
			projectFile.AddLine("NMAKE=" + NMakeCmdLine);
			projectFile.AddLine("CPP=" + CppPath);
			projectFile.AddLine("MAKE=" + MakeCmdLine);
			projectFile.AddLine(wxString::Format("WINTOOEM=%d", WinToOEM));
			projectFile.Write();
			projectFile.Close();
			return true;
		}
		else {
			return false;
		}
	}
};