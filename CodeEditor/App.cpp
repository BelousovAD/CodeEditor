#include "App.h"
#include "MainWindow.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    MainWindow* mainWindow = new MainWindow(nullptr, ID_MainWindow, _("Code Editor"));
    mainWindow->Show();

    return true;
}
