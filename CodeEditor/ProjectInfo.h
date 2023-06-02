#pragma once
#include <vector>
#include "wx/string.h"
#include "wx/textfile.h"

#include "defsexternal.h"

struct ProjectInfo {
	wxString directory = "";
	wxString filepath = "";
	std::vector<wxString> files;
	wxString mainFile = "";
	wxString execFile = "";
	int target = 0;
	bool ProjectHaveChanges = false;

	bool HasFile(const wxString& filename) {
		std::vector<wxString>::iterator iter;
		iter = std::find(files.begin(), files.end(), filename);
		return iter != files.end();
	}

	bool LoadProjectInfo() {
		wxTextFile projectFile(filepath);
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
				else if ("FILE" == key) {
					files.push_back(value);
				}
				else if ("MAIN" == key) {
					mainFile = value;
				}
				else if ("EXEC" == key) {
					execFile = value;
				}
				else if ("TARGET" == key) {
					unsigned int i;
					value.ToUInt(&i);
					if (i < Targets.size()) {
						target = i;
					}
				}
			}
			projectFile.Close();
			return true;
		}
		else {
			return false;
		}
	}

	bool SaveProjectInfo() {
		wxTextFile projectFile(filepath);
		if (projectFile.Open()) {
			projectFile.Clear();
			projectFile.AddLine("MAIN=" + mainFile);
			projectFile.AddLine("EXEC=" + execFile);
			for (int i = 0; i < files.size(); i++) {
				projectFile.AddLine("FILE=" + files[i]);
			}
			projectFile.AddLine(wxString::Format("TARGET=%d", target));
			projectFile.Write();
			projectFile.Close();
			return true;
		}
		else {
			return false;
		}
	}
};