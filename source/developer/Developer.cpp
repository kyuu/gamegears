#include <wx/aboutdlg.h>
#include "Developer.hpp"
#include "FileExplorer.hpp"
#include "ScriptEditor.hpp"
#include "ICommandHandlerBridge.hpp"

#include <iostream>
#include <fstream>
extern std::ofstream Log("log.txt");


//-------------------------------------------------------------------
Developer::Developer()
    : wxFrame(0, -1, wxT("GameGears Developer"))
    , _documentContainer(0)
    , _projectDataExplorer(0)
    , _commonDataExplorer(0)
    , _toolbar(0)
    , _menubar(0)
    , _statusbar(0)
{
    // set interval for UI update events
    wxUpdateUIEvent::SetUpdateInterval(100);

    // connect events
    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Developer::OnClose));

    Connect(ID_QUIT,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::OnQuit));
    Connect(ID_ABOUT,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::OnAbout));

    Connect(ID_PROJECT_DATA_EXPLORER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::OnViewProjectDataExplorer));
    Connect(ID_COMMON_DATA_EXPLORER,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::OnViewCommonDataExplorer));
    Connect(ID_STANDARD_TOOLBAR,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::OnViewStandardToolbar));

    Connect(ID_CUT,      wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));
    Connect(ID_COPY,     wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));
    Connect(ID_PASTE,    wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));
    Connect(ID_UNDO,     wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));
    Connect(ID_REDO,     wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));
    Connect(ID_ZOOM_IN,  wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));
    Connect(ID_ZOOM_OUT, wxEVT_COMMAND_TOOL_CLICKED,  wxCommandEventHandler(Developer::onEditorCommand));

    Connect(ID_CUT,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);
    Connect(ID_COPY,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);
    Connect(ID_PASTE,    wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);
    Connect(ID_UNDO,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);
    Connect(ID_REDO,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);
    Connect(ID_ZOOM_IN,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);
    Connect(ID_ZOOM_OUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onCommandUpdateUI), 0, this);

    Connect(ID_PROJECT_DATA_EXPLORER, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onViewUpdateUI), 0, this);
    Connect(ID_COMMON_DATA_EXPLORER,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onViewUpdateUI), 0, this);
    Connect(ID_STANDARD_TOOLBAR,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onViewUpdateUI), 0, this);

    // load preferences
    _prefs.load("developer.prefs");

    // notify wxAUI which frame to use
    _windowManager.SetManagedWindow(this);

    // create editor container
    _documentContainer = new wxAuiNotebook(this);
    _windowManager.AddPane(_documentContainer, wxAuiPaneInfo().Name("DocumentContainer").CenterPane());

    // create project data explorer
    _projectDataExplorer = new FileExplorer(this, "Project Data", _prefs.System.PathToData);

    wxAuiPaneInfo projectDataExplorerPaneInfo;
    projectDataExplorerPaneInfo.Name("ProjectDataExplorer");
    projectDataExplorerPaneInfo.Left();
    projectDataExplorerPaneInfo.Floatable();
    projectDataExplorerPaneInfo.Dockable();
    projectDataExplorerPaneInfo.PinButton();
    projectDataExplorerPaneInfo.MaximizeButton();
    projectDataExplorerPaneInfo.MinimizeButton();
    projectDataExplorerPaneInfo.Gripper();
    projectDataExplorerPaneInfo.GripperTop();
    projectDataExplorerPaneInfo.DestroyOnClose(false);
    projectDataExplorerPaneInfo.MinSize(100, 100);

    _windowManager.AddPane(_projectDataExplorer, projectDataExplorerPaneInfo);

    // create common data explorer
    _commonDataExplorer = new FileExplorer(this, "Common Data", _prefs.System.PathToCommonData);

    wxAuiPaneInfo commonDataExplorerPaneInfo;
    commonDataExplorerPaneInfo.Name("CommonDataExplorer");
    commonDataExplorerPaneInfo.Left();
    commonDataExplorerPaneInfo.Floatable();
    commonDataExplorerPaneInfo.Dockable();
    commonDataExplorerPaneInfo.PinButton();
    commonDataExplorerPaneInfo.MaximizeButton();
    commonDataExplorerPaneInfo.MinimizeButton();
    commonDataExplorerPaneInfo.Gripper();
    commonDataExplorerPaneInfo.GripperTop();
    commonDataExplorerPaneInfo.DestroyOnClose(false);
    commonDataExplorerPaneInfo.MinSize(100, 100);

    _windowManager.AddPane(_commonDataExplorer, commonDataExplorerPaneInfo);

    // create source editor
    ScriptEditor* sourceEditor = new ScriptEditor(this, "data/script/game.nut");
    _documentContainer->AddPage(sourceEditor, wxT("game.nut"));

    // create toolbar
    _toolbar = new wxAuiToolBar(this);

        _toolbar->AddTool(ID_NEW_PROJECT,  wxEmptyString, wxBitmap(wxT("system/new-project.png"), wxBITMAP_TYPE_PNG), wxT("New Project"));
        _toolbar->AddTool(ID_NEW_CONTENT,  wxEmptyString, wxBitmap(wxT("system/new-content.png"), wxBITMAP_TYPE_PNG), wxT("New Content"));
        _toolbar->AddTool(ID_OPEN_FILE,    wxEmptyString, wxBitmap(wxT("system/open-file.png"),   wxBITMAP_TYPE_PNG), wxT("Open File"));
        _toolbar->AddTool(ID_SAVE,         wxEmptyString, wxBitmap(wxT("system/save.png"),        wxBITMAP_TYPE_PNG), wxT("Save"));
        _toolbar->AddTool(ID_SAVE_ALL,     wxEmptyString, wxBitmap(wxT("system/save-all.png"),    wxBITMAP_TYPE_PNG), wxT("Save All"));

        _toolbar->AddSeparator();

        _toolbar->AddTool(ID_CUT,          wxEmptyString, wxBitmap(wxT("system/cut.png"),   wxBITMAP_TYPE_PNG), wxT("Cut"));
        _toolbar->AddTool(ID_COPY,         wxEmptyString, wxBitmap(wxT("system/copy.png"),  wxBITMAP_TYPE_PNG), wxT("Copy"));
        _toolbar->AddTool(ID_PASTE,        wxEmptyString, wxBitmap(wxT("system/paste.png"), wxBITMAP_TYPE_PNG), wxT("Paste"));

        _toolbar->AddSeparator();

        _toolbar->AddTool(ID_UNDO,         wxEmptyString, wxBitmap(wxT("system/undo.png"), wxBITMAP_TYPE_PNG), wxT("Undo"));
        _toolbar->AddTool(ID_REDO,         wxEmptyString, wxBitmap(wxT("system/redo.png"), wxBITMAP_TYPE_PNG), wxT("Redo"));

        _toolbar->AddSeparator();

        _toolbar->AddTool(ID_DEBUG,        wxEmptyString, wxBitmap(wxT("system/debug.png"), wxBITMAP_TYPE_PNG), wxT("Start Debugging"));

        _toolbar->AddSeparator();

        _toolbar->AddTool(ID_ZOOM_IN,      wxEmptyString, wxBitmap(wxT("system/zoom-in.png"), wxBITMAP_TYPE_PNG), wxT("Zoom In"));
        _toolbar->AddTool(ID_ZOOM_OUT,     wxEmptyString, wxBitmap(wxT("system/zoom-out.png"), wxBITMAP_TYPE_PNG), wxT("Zoom Out"));

    wxAuiPaneInfo standardToolbarPaneInfo;
    standardToolbarPaneInfo.Name("StandardToolbar");
    standardToolbarPaneInfo.Top();
    standardToolbarPaneInfo.ToolbarPane();

    _windowManager.AddPane(_toolbar, standardToolbarPaneInfo);

    // create menubar
    _menubar = new wxMenuBar();

        wxMenu* fileMenu = new wxMenu();

            wxMenu* newSubMenu = new wxMenu();
                newSubMenu->Append(ID_NEW_PROJECT, _("&Project"));
                newSubMenu->Append(ID_NEW_CONTENT, _("&File"));
            fileMenu->AppendSubMenu(newSubMenu, _("&New"));

            wxMenu* openSubMenu = new wxMenu();
                openSubMenu->Append(ID_OPEN_PROJECT, _("&Project"));
                openSubMenu->Append(ID_OPEN_FILE,    _("&File"));
            fileMenu->AppendSubMenu(openSubMenu, _("&Open"));

            fileMenu->AppendSeparator();

            fileMenu->Append(ID_CLOSE, _("&Close"));
            fileMenu->Append(ID_CLOSE_PROJECT, _("Close &Project"));

            fileMenu->AppendSeparator();

            fileMenu->Append(ID_SAVE,     _("&Save"));
            fileMenu->Append(ID_SAVE_AS,  _("Save A&s..."));
            fileMenu->Append(ID_SAVE_ALL, _("Save A&ll"));

            fileMenu->AppendSeparator();

            wxMenu* recentFilesSubMenu = new wxMenu();
            wxMenuItem* recentFilesSubMenuItem = fileMenu->AppendSubMenu(recentFilesSubMenu, _("&Recent Files"));
            for (size_t i = 0; i < 10 && i < _prefs.System.RecentFiles.size(); i++) {
                recentFilesSubMenu->Append(ID_RECENT_FILE_0 + i, _prefs.System.RecentFiles[i]);
            }
            if (_prefs.System.RecentFiles.size() == 0) {
                recentFilesSubMenuItem->Enable(false);
            }

            wxMenu* recentProjectsSubMenu = new wxMenu();
            wxMenuItem* recentProjectsSubMenuItem = fileMenu->AppendSubMenu(recentProjectsSubMenu, _("&Recent Projects"));
            for (size_t i = 0; i < 10 && i < _prefs.System.RecentProjects.size(); i++) {
                recentProjectsSubMenu->Append(ID_RECENT_PROJECT_0 + i, _prefs.System.RecentProjects[i]);
            }
            if (_prefs.System.RecentProjects.size() == 0) {
                recentProjectsSubMenuItem->Enable(false);
            }

            fileMenu->AppendSeparator();

            fileMenu->Append(ID_QUIT, _("E&xit"));

        _menubar->Append(fileMenu, _("&File"));

        wxMenu* editMenu = new wxMenu();

            editMenu->Append(ID_UNDO, _("&Undo"));
            editMenu->Append(ID_REDO, _("&Redo"));

            editMenu->AppendSeparator();

            editMenu->Append(ID_CUT,   _("&Cut"));
            editMenu->Append(ID_COPY,  _("Co&py"));
            editMenu->Append(ID_PASTE, _("&Paste"));

        _menubar->Append(editMenu, _("&Edit"));

        wxMenu* viewMenu = new wxMenu();

            viewMenu->Append(ID_ZOOM_IN,  _("&Zoom In"));
            viewMenu->Append(ID_ZOOM_OUT, _("&Zoom Out"));

            viewMenu->AppendSeparator();

            viewMenu->AppendCheckItem(ID_PROJECT_DATA_EXPLORER, _("&Project Data Explorer"));
            viewMenu->AppendCheckItem(ID_COMMON_DATA_EXPLORER,  _("&Common Data Explorer"));

            wxMenu* toolbarsSubMenu = new wxMenu();
                toolbarsSubMenu->AppendCheckItem(ID_STANDARD_TOOLBAR, _("&Standard"));
            viewMenu->AppendSubMenu(toolbarsSubMenu, _("&Toolbars"));

        _menubar->Append(viewMenu, _("&View"));

        wxMenu* projectMenu = new wxMenu();
            projectMenu->Append(ID_PROJECT_PROPERTIES,  _("&Properties"));
        _menubar->Append(projectMenu, _("&Project"));

        wxMenu* toolsMenu = new wxMenu();
            toolsMenu->Append(ID_PREFERENCES,  _("&Preferences"));
        _menubar->Append(toolsMenu, _("&Tools"));

        wxMenu* helpMenu = new wxMenu();
            helpMenu->Append(ID_ABOUT, _("&About GameGears Developer..."));
        _menubar->Append(helpMenu, _("&Help"));

    SetMenuBar(_menubar);

    // create statusbar
    _statusbar = new wxStatusBar(this);
    SetStatusBar(_statusbar);

    // update manager
    _windowManager.Update();

    // set window size and position
    SetSize(
        _prefs.System.Window.X,
        _prefs.System.Window.Y,
        _prefs.System.Window.Width,
        _prefs.System.Window.Height
    );

    // maximize window if preferences say so
    if (_prefs.System.Window.Maximized) {
        Maximize(true);
    }

    // load icon
    wxIcon icon;
    icon.LoadFile(wxT("system/developer.ico"), wxBITMAP_TYPE_ICO);
    if (icon.IsOk()) {
        SetIcon(icon);
    }

    // load pane info
    _windowManager.LoadPerspective(_prefs.System.PaneInfo);
}

//-------------------------------------------------------------------
Developer::~Developer()
{
    // deinitialize the frame manager
    _windowManager.UnInit();
}

//-------------------------------------------------------------------
Preferences&
Developer::getPreferences()
{
    return _prefs;
}

//-------------------------------------------------------------------
void
Developer::OnClose(wxCloseEvent& event)
{
    _prefs.System.PaneInfo = _windowManager.SavePerspective().ToStdString();

    _prefs.System.Window.Maximized = IsMaximized();

    if (!IsMaximized()) {
        _prefs.System.Window.Width  = GetSize().GetWidth();
        _prefs.System.Window.Height = GetSize().GetHeight();
        _prefs.System.Window.X      = GetPosition().x;
        _prefs.System.Window.Y      = GetPosition().y;
    }

    // save preferences
    _prefs.save("developer.prefs");

    event.Skip();
}

//-------------------------------------------------------------------
void
Developer::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//-------------------------------------------------------------------
void
Developer::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo aboutInfo;

    aboutInfo.SetName("GameGears Developer");
    aboutInfo.SetVersion(wxT("0.1 Alpha"));
    aboutInfo.SetCopyright(wxT("\u00A9 2012 Anatoli Steinmark"));
    aboutInfo.AddDeveloper("Anatoli Steinmark");

    wxAboutBox(aboutInfo);
}

//-------------------------------------------------------------------
void
Developer::OnViewProjectDataExplorer(wxCommandEvent& e)
{
    wxAuiPaneInfo& projectDataExplorer = _windowManager.GetPane("ProjectDataExplorer");
    if (projectDataExplorer.IsShown()) {
        projectDataExplorer.Hide();
    } else {
        projectDataExplorer.Show();
    }
    _windowManager.Update();
}

//-------------------------------------------------------------------
void
Developer::OnViewCommonDataExplorer(wxCommandEvent& e)
{
    wxAuiPaneInfo& commonDataExplorer = _windowManager.GetPane("CommonDataExplorer");
    if (commonDataExplorer.IsShown()) {
        commonDataExplorer.Hide();
    } else {
        commonDataExplorer.Show();
    }
    _windowManager.Update();
}

//-------------------------------------------------------------------
void
Developer::OnViewStandardToolbar(wxCommandEvent& e)
{
    wxAuiPaneInfo& standardToolbar = _windowManager.GetPane("StandardToolbar");
    if (standardToolbar.IsShown()) {
        standardToolbar.Hide();
    } else {
        standardToolbar.Show();
    }
    _windowManager.Update();
}

//-------------------------------------------------------------------
void
Developer::onEditorCommand(wxCommandEvent& e)
{
    wxWindow* currentEditor = _documentContainer->GetCurrentPage();
    if (currentEditor) {
        ICommandHandlerBridge* commandHandlerBridge = (ICommandHandlerBridge*)currentEditor->GetClientData();
        commandHandlerBridge->handleCommand(e.GetId());
    }
}

//-------------------------------------------------------------------
void
Developer::onCommandUpdateUI(wxUpdateUIEvent& e)
{
    wxWindow* currentEditor = _documentContainer->GetCurrentPage();
    if (currentEditor) {
        ICommandHandlerBridge* commandHandlerBridge = (ICommandHandlerBridge*)(currentEditor->GetClientData());

        if (commandHandlerBridge->canDoCommand(e.GetId())) {
            e.Enable(true);
            return;
        }
    }
    e.Enable(false);
}

//-------------------------------------------------------------------
void
Developer::onViewUpdateUI(wxUpdateUIEvent& e)
{
    switch (e.GetId()) {
    case ID_PROJECT_DATA_EXPLORER:
        if (_windowManager.GetPane("ProjectDataExplorer").IsShown()) {
            e.Check(true);
        } else {
            e.Check(false);
        }
        break;
    case ID_COMMON_DATA_EXPLORER:
        if (_windowManager.GetPane("CommonDataExplorer").IsShown()) {
            e.Check(true);
        } else {
            e.Check(false);
        }
        break;
    case ID_STANDARD_TOOLBAR:
        if (_windowManager.GetPane("StandardToolbar").IsShown()) {
            e.Check(true);
        } else {
            e.Check(false);
        }
        break;
    }
}
