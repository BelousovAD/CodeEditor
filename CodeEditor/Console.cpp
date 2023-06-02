#include "Console.h"

#include "wx/log.h"

Console::Console(wxWindow* parent, wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	int n, const wxString choices[],
	long style,
	const wxValidator& validator,
	const wxString& name)
	: wxListBox(parent, id, pos, size, n, choices, style, validator, name) {
	wxFont font(wxFontInfo(12).Family(wxFONTFAMILY_TELETYPE));
	if (font.IsOk()) {
		this->SetFont(font);
	}
}

Console::~Console() {
	this->Clear();
}

void Console::ShowOutput(const wxString& cmd, const wxArrayString& output, const wxString& title, const wxString& appName) {
    size_t count = output.GetCount();
    if (!count) {
        return;
    }

    this->Append(wxString::Format("--- %s of '%s' ---", title, appName));

    for (size_t i = 0; i < count; i++)
    {
        this->Append(output[i]);
    }

    this->Append(wxString::Format("--- End of %s ---", title.Lower()));
}
