#include "Developer.hpp"


class App : public wxApp {
    virtual bool OnInit();
};

//-------------------------------------------------------------------
bool
App::OnInit()
{
    // register PNG handler (for icon loading)
    wxImage::AddHandler(new wxPNGHandler);

    // setup main window
    Developer* developer = new Developer();
    SetTopWindow(developer);
    developer->Show(true);

    return true;
}

//-------------------------------------------------------------------
IMPLEMENT_APP(App)
