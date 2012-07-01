/*
    This file is part of GameGears.

    GameGears is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GameGears is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GameGears.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <sstream>
#include <wx/aboutdlg.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include "Developer.hpp"
#include "PreferencesDialog.hpp"
#include "FileExplorer.hpp"
#include "Output.hpp"
#include "MenuBar.hpp"
#include "StatusBar.hpp"
#include "StandardToolBar.hpp"
#include "PaneToolBar.hpp"
#include "ProjectToolBar.hpp"
#include "TextToolBar.hpp"
#include "TextEditor.hpp"
#include "NewProjectDialog.hpp"
#include "NewFileDialog.hpp"
#include "AddNewFileDialog.hpp"
#include "ProjectPropertiesDialog.hpp"
#include "SaveChangesDialog.hpp"
#include "version.hpp"
#include "commands.hpp"


//-------------------------------------------------------------------
Developer::Developer()
    : wxFrame(0, wxID_ANY, wxEmptyString)
    , _initialPath(wxGetCwd())
    , _windowManager(this, wxAUI_MGR_DEFAULT)
    , _editorContainer(0)
    , _statusBar(0)
    , _menuBar(0)
    , _standardToolBar(0)
    , _paneToolBar(0)
    , _projectToolBar(0)
    , _textToolBar(0)
    , _projectDataExplorer(0)
    , _commonDataExplorer(0)
    , _output(0)
    , _isProjectOpen(false)
    , _projectFullScreen(false)
{
    // set interval for UI update events
    wxUpdateUIEvent::SetUpdateInterval(100);

    // load preferences
    _preferences.loadFromFile("system/developer/developer.prefs");

    // create status bar
    _statusBar = new StatusBar(this);
    SetStatusBar(_statusBar);

    // create menu bar
    _menuBar = new MenuBar(this);
    SetMenuBar(_menuBar);

    // create panes
    _editorContainer     = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_WINDOWLIST_BUTTON);
    _standardToolBar     = new StandardToolBar(this);
    _paneToolBar         = new PaneToolBar(this);
    _projectToolBar      = new ProjectToolBar(this);
    _textToolBar         = new TextToolBar(this);
    _output              = new Output(this);
    _projectDataExplorer = new FileExplorer(this);

    wxString pathToCommonData = _preferences.General.CommonDataLocation;
    if (wxFileName(pathToCommonData).IsRelative()) {
        pathToCommonData = _initialPath + wxFileName::GetPathSeparator() + pathToCommonData;
    }
    _commonDataExplorer = new FileExplorer(this, pathToCommonData);

    // add panes to window manager
    _windowManager.AddPane(_editorContainer,     getFileEditorContainerPaneInfo());

    _windowManager.AddPane(_standardToolBar,     getStandardToolBarPaneInfo());
    _windowManager.AddPane(_paneToolBar,         getPaneToolBarPaneInfo());
    _windowManager.AddPane(_projectToolBar,      getProjectToolBarPaneInfo());
    _windowManager.AddPane(_textToolBar,         getTextToolBarPaneInfo());

    _windowManager.AddPane(_projectDataExplorer, getProjectDataExplorerPaneInfo());
    _windowManager.AddPane(_commonDataExplorer,  getCommonDataExplorerPaneInfo());
    _windowManager.AddPane(_output,              getOutputPaneInfo());

    // update window manager
    _windowManager.Update();

    // restore window layout
    _windowManager.LoadPerspective(_preferences.General.DockManagerState);

    // restore window properties
    SetSize(
        _preferences.General.Window.X,
        _preferences.General.Window.Y,
        _preferences.General.Window.Width,
        _preferences.General.Window.Height
    );
    if (_preferences.General.Window.Maximized) {
        Maximize(true);
    }

    // set icon
    SetIcon(wxIcon(wxT("system/developer/icons/developer.ico"), wxBITMAP_TYPE_ICO));

    // set title
    updateTitle();

    // open last project if desired
    if (_preferences.General.OpenLastProjectOnStartup) {
        wxArrayString& recentProjects = _preferences.General.RecentProjects;
        if (recentProjects.GetCount() > 0) {
            openProject(recentProjects.Item(0));
        }
    }

    // connect command events
    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Developer::onClose));

    Connect(CMD_EXIT_DEVELOPER,             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onExitDeveloper));
    Connect(CMD_SHOW_ABOUT_DEVELOPER,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onShowAboutDeveloper));
    Connect(CMD_EDIT_DEVELOPER_PREFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onEditDeveloperPreferences));

    Connect(CMD_SHOW_PROJECT_DATA_EXPLORER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onShowProjectDataExplorer));
    Connect(CMD_SHOW_COMMON_DATA_EXPLORER,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onShowCommonDataExplorer));
    Connect(CMD_SHOW_OUTPUT,                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onShowOutput));

    Connect(CMD_CREATE_NEW_PROJECT,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onCreateNewProject));
    Connect(CMD_OPEN_PROJECT,               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenProject));
    Connect(CMD_OPEN_RECENT_PROJECT_1,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_2,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_3,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_4,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_5,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_6,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_7,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_8,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_9,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_OPEN_RECENT_PROJECT_10,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentProject));
    Connect(CMD_PROJECT_ADD_NEW_FILE,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onProjectAddNewFile));
    Connect(CMD_START_PROJECT,              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onStartProject));
    Connect(CMD_PROJECT_FULLSCREEN,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onProjectFullScreen));
    Connect(CMD_EDIT_PROJECT_PROPERTIES,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onEditProjectProperties));
    Connect(CMD_CLOSE_PROJECT,              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onCloseProject));

    Connect(CMD_CREATE_NEW_FILE,            wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onCreateNewFile));
    Connect(CMD_OPEN_FILE,                  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenFile));
    Connect(CMD_OPEN_RECENT_FILE_1,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_2,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_3,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_4,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_5,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_6,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_7,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_8,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_9,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_OPEN_RECENT_FILE_10,        wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onOpenRecentFile));
    Connect(CMD_SAVE_FILE,                  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onSaveFile));
    Connect(CMD_SAVE_FILE_AS,               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onSaveFileAs));
    Connect(CMD_SAVE_ALL_FILES,             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onSaveAllFiles));
    Connect(CMD_CLOSE_FILE,                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onCloseFile));
    Connect(CMD_CLOSE_ALL_FILES,            wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onCloseAllFiles));

    Connect(CMD_UNDO,                       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_REDO,                       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_CUT,                        wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_COPY,                       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_PASTE,                      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_ZOOM_IN,               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_ZOOM_OUT,              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_COMMENT_OUT,           wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_UNCOMMENT,             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_INDENT,                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_OUTDENT,               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_MOVE_UP,               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_MOVE_DOWN,             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));
    Connect(CMD_TEXT_GOTO,                  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Developer::onFileEditorCommand));

    // connect update UI events
    Connect(CMD_SHOW_PROJECT_DATA_EXPLORER, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_ShowProjectDataExplorer));
    Connect(CMD_SHOW_COMMON_DATA_EXPLORER,  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_ShowCommonDataExplorer));
    Connect(CMD_SHOW_OUTPUT,                wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_ShowOutput));

    Connect(CMD_PROJECT_ADD_NEW_FILE,       wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_ProjectAddNewFile));
    Connect(CMD_START_PROJECT,              wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_StartProject));
    Connect(CMD_PROJECT_FULLSCREEN,         wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_ProjectFullScreen));
    Connect(CMD_EDIT_PROJECT_PROPERTIES,    wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_EditProjectProperties));
    Connect(CMD_CLOSE_PROJECT,              wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_CloseProject));

    Connect(CMD_SAVE_FILE,                  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_SaveFile));
    Connect(CMD_SAVE_FILE_AS,               wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_SaveFileAs));
    Connect(CMD_SAVE_ALL_FILES,             wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_SaveAllFiles));
    Connect(CMD_CLOSE_FILE,                 wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_CloseFile));
    Connect(CMD_CLOSE_ALL_FILES,            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_CloseAllFiles));

    Connect(CMD_UNDO,                       wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_REDO,                       wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_CUT,                        wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_COPY,                       wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_PASTE,                      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_ZOOM_IN,               wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_ZOOM_OUT,              wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_COMMENT_OUT,           wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_UNCOMMENT,             wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_INDENT,                wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_OUTDENT,               wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_MOVE_UP,               wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_MOVE_DOWN,             wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));
    Connect(CMD_TEXT_GOTO,                  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Developer::onUpdateUI_FileEditorCommand));

    // connect AUI notebook events
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE,   wxAuiNotebookEventHandler(Developer::onFileEditorClose));
    Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(Developer::onFileEditorSelectionChanged));
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
    return _preferences;
}

//-------------------------------------------------------------------
bool
Developer::isProjectOpen() const
{
    return _isProjectOpen;
}

//-------------------------------------------------------------------
const wxString&
Developer::getProjectLocation() const
{
    return _projectLocation;
}

//-------------------------------------------------------------------
bool
Developer::createNewProject()
{
    // construct default project location
    wxFileName defaultLocation(_preferences.General.DataLocation);
    if (defaultLocation.IsRelative()) {
        defaultLocation = _initialPath + wxFileName::GetPathSeparator() + defaultLocation.GetFullPath();
    }

    // show new project dialog
    NewProjectDialog npd(defaultLocation.GetFullPath());
    if (npd.ShowModal() == wxID_OK) { // user clicked ok
        wxString projectLocation = npd.getProjectLocation();
        wxString projectFile     = projectLocation + wxFileName::GetPathSeparator() + wxT("game.ggproj");

        // if the project location does not exist, create it
        if (!wxDir::Exists(projectLocation)) {
            if (!wxDir::Make(projectLocation, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
                wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
                ed.SetMessage(wxT("Could not create project location '") + projectLocation + wxT("'."));
                ed.ShowModal();
                return false;
            }
        } else {
            // make sure we don't overwrite an existing project
            if (wxFileName::FileExists(projectFile)) {
                wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
                ed.SetMessage(wxT("Project location '") + projectLocation + wxT("' already contains a project."));
                ed.ShowModal();
                return false;
            }
        }

        // create project file
        ProjectProperties pp = npd.getProjectProperties();
        if (!pp.saveToFile(projectFile.ToStdString())) {
            wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            ed.SetMessage(wxT("Could not create project file '") + projectFile + wxT("'."));
            ed.ShowModal();
            return false;
        }

        // open the new project
        openProject(projectFile);
    }

    return true;
}

//-------------------------------------------------------------------
bool
Developer::openProject(const wxString& projectFile)
{
    // ensure that the file is a project file
    if (wxFileName(projectFile).GetFullName() != wxT("game.ggproj")) {
        wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        ed.SetMessage(wxT("Could not open project '") + projectFile + wxT("'. Invalid project file."));
        ed.ShowModal();
        return false;
    }

    // ensure that the file exists
    if (!wxFileName::FileExists(projectFile)) {
        wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        ed.SetMessage(wxT("Could not open project '") + projectFile + wxT("'. Project file does not exist."));
        ed.ShowModal();
        return false;
    }

    wxString projectLocation = wxFileName(projectFile).GetPath();

    // if project is already open, nothing to do here
    if (_projectLocation == projectLocation) {
        return true;
    }

    ProjectProperties projectProperties;

    // load project properties
    if (!projectProperties.loadFromFile(projectFile.ToStdString())) {
        wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        ed.SetMessage(wxT("Could not open project '") + projectFile + wxT("'. Error loading project file."));
        ed.ShowModal();
        return false;
    }

    // if a project is open, close it
    if (!closeProject()) {
        return false;
    }

    // init internal variables
    _isProjectOpen   = true;
    _projectName     = projectProperties.Name;
    _projectLocation = projectLocation;

    // set up project data explorer
    _projectDataExplorer->initFileTree(projectLocation);
    showProjectDataExplorer();

    // update recent projects
    wxArrayString& recentProjects = _preferences.General.RecentProjects;
    if (recentProjects.IsEmpty() || projectFile != recentProjects.Item(0)) {
        // insert current project
        recentProjects.Insert(projectFile, 0);

        // ensure recent projects list contains at max 10 projects
        if (recentProjects.GetCount() > 10) {
            recentProjects.RemoveAt(recentProjects.GetCount()-1);
        }

        // update file menu
        _menuBar->updateFileMenu();
    }

    // update title
    updateTitle();

    return true;
}

//-------------------------------------------------------------------
bool
Developer::closeProject()
{
    if (_isProjectOpen) {
        // close all files
        if (_editorContainer->GetPageCount() > 0) {
            // see if any files are modified and add them to the item list
            wxArrayString itemList;

            size_t numEditors = _editorContainer->GetPageCount();
            for (size_t i = 0; i < numEditors; i++) {
                wxWindow* fileEditor = _editorContainer->GetPage(i);
                assert(fileEditor);

                IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
                assert(delegator);

                if (delegator->isFileModified()) {
                    itemList.Add(wxFileName(delegator->getFileName()).GetFullName());
                }
            }

            // if there are any modified files, ask if we should save the changes
            if (itemList.GetCount() > 0) {
                // show save changes dialog
                SaveChangesDialog scd(itemList);
                int result = scd.ShowModal();

                if (result == wxID_YES) {
                    // save the changes
                    for (size_t i = 0; i < numEditors; i++) {
                        wxWindow* fileEditor = _editorContainer->GetPage(i);
                        assert(fileEditor);

                        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
                        assert(delegator);

                        if (delegator->isFileModified()) {
                            delegator->saveFile();
                        }
                    }
                }

                if (result == wxID_CANCEL) {
                    // cancel
                    return false;
                }
            }

            // close all files
            _editorContainer->DeleteAllPages();
            showFileEditorToolBar(FET_TEXT, false);
        }

        // reset internal variables
        _isProjectOpen = false;
        _projectName.Clear();
        _projectLocation.Clear();

        // update title
        updateTitle();

        // reset project data explorer
        _projectDataExplorer->clearFileTree();
        showProjectDataExplorer(false);
    }

    return true;
}

//-------------------------------------------------------------------
bool
Developer::createNewFile()
{
    NewFileDialog nfd;
    if (nfd.ShowModal() == wxID_CANCEL) {
        return false;
    }

    int fileType = nfd.getSelectedFileType();

    switch (fileType) {
        case FT_SCRIPT: {
            TextEditor* textEditor = new TextEditor(this, _editorContainer, wxT("NewScript"), true);
            _editorContainer->AddPage(textEditor, wxT("NewScript"), true);
        }
        break;
        case FT_TEXT: {
            TextEditor* textEditor = new TextEditor(this, _editorContainer, wxT("NewText"));
            _editorContainer->AddPage(textEditor, wxT("NewText"), true);
        }
        break;
        default:
            return false;
    }

    return true;
}

//-------------------------------------------------------------------
bool
Developer::addNewFile(const wxString& defaultLocation)
{
    AddNewFileDialog anfd(defaultLocation);
    if (anfd.ShowModal() == wxID_CANCEL) {
        return false;
    }

    int fileType = anfd.getSelectedFileType();
    wxString fileName = anfd.getFileName();

    switch (fileType) {
        case FT_SCRIPT: {
            TextEditor* textEditor = new TextEditor(this, _editorContainer, fileName, true);
            _editorContainer->AddPage(textEditor, wxFileName(fileName).GetFullName(), true);
            return textEditor->saveFileAs(fileName);
        }
        break;
        case FT_TEXT: {
            TextEditor* textEditor = new TextEditor(this, _editorContainer, fileName);
            _editorContainer->AddPage(textEditor, wxFileName(fileName).GetFullName(), true);
            return textEditor->saveFileAs(fileName);
        }
        break;
        default:
            return false;
    }
}

//-------------------------------------------------------------------
bool
Developer::openFile(const wxString& fileName)
{
    int fileType = getFileType(fileName);
    return openFileAs(fileName, fileType);
}

//-------------------------------------------------------------------
bool
Developer::openFileAs(const wxString& fileName, int fileType)
{
    // see if the file exists
    if (!wxFileName::FileExists(fileName)) {
        wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        ed.SetMessage(wxT("Could not open file '") + fileName + wxT("'. File does not exist."));
        ed.ShowModal();
        return false;
    }

    // see first if the file is already opened
    size_t numEditors = _editorContainer->GetPageCount();
    for (size_t i = 0; i < numEditors; i++) {
        wxWindow* fileEditor = _editorContainer->GetPage(i);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)fileEditor->GetClientData();
        assert(delegator);

        if (fileName == delegator->getFileName()) {
            // file already opened, just select it and return
            _editorContainer->SetSelection(i);
            return true;
        }
    }

    // file not yet open, so open it
    switch (fileType) {
        case FT_PROJECT:
        case FT_SCRIPT: {
            TextEditor* textEditor = new TextEditor(this, _editorContainer, fileName, true);
            _editorContainer->AddPage(textEditor, wxFileName(fileName).GetFullName(), true);
        }
        break;
        case FT_TEXT: {
            TextEditor* textEditor = new TextEditor(this, _editorContainer, fileName);
            _editorContainer->AddPage(textEditor, wxFileName(fileName).GetFullName(), true);
        }
        break;
        default: {
            wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            ed.SetMessage(wxT("Could not open file '") + fileName + wxT("'. Unrecognized file type."));
            ed.ShowModal();
            return false;
        }
    }

    // update recent files
    wxArrayString& recentFiles = _preferences.General.RecentFiles;
    if (recentFiles.IsEmpty() || fileName != recentFiles.Item(0)) {
        // insert file
        recentFiles.Insert(fileName, 0);

        // ensure recent files list contains at max 10 files
        if (recentFiles.GetCount() > 10) {
            recentFiles.RemoveAt(recentFiles.GetCount()-1);
        }

        // update file menu
        _menuBar->updateFileMenu();
    }

    return true;
}

//-------------------------------------------------------------------
void
Developer::fileEditorSavePointReached(wxWindow* fileEditor)
{
    IFileEditorDelegator* delegator = (IFileEditorDelegator*)fileEditor->GetClientData();
    assert(delegator);

    int pageIndex = _editorContainer->GetPageIndex(fileEditor);
    assert(pageIndex != wxNOT_FOUND);

    _editorContainer->SetPageText(pageIndex, wxFileName(delegator->getFileName()).GetFullName());
}

//-------------------------------------------------------------------
void
Developer::fileEditorSavePointLeft(wxWindow* fileEditor)
{
    IFileEditorDelegator* delegator = (IFileEditorDelegator*)fileEditor->GetClientData();
    assert(delegator);

    int pageIndex = _editorContainer->GetPageIndex(fileEditor);
    assert(pageIndex != wxNOT_FOUND);

    _editorContainer->SetPageText(pageIndex, wxFileName(delegator->getFileName()).GetFullName() + wxT("*"));
}

//-------------------------------------------------------------------
void
Developer::fileEditorFileSaved(wxWindow* fileEditor)
{
    IFileEditorDelegator* delegator = (IFileEditorDelegator*)fileEditor->GetClientData();
    assert(delegator);

    // update page text
    int pageIndex = _editorContainer->GetPageIndex(fileEditor);
    assert(pageIndex != wxNOT_FOUND);

    wxString fileName = delegator->getFileName();
    _editorContainer->SetPageText(pageIndex, wxFileName(fileName).GetFullName());

    // notify the file explorers
    _projectDataExplorer->fileSaved(fileName);
    _commonDataExplorer->fileSaved(fileName);
}

//-------------------------------------------------------------------
int
Developer::getFileType(const wxString& fileName)
{
    // first see if it's a directory
    if (wxDir::Exists(fileName)) {
        return FT_DIRECTORY;
    }

    // now look at the file extension
    wxString ext = wxFileName(fileName).GetExt();

    // see if it's a project file
    wxArrayString& projectFileTypes = _preferences.General.FileTypeAssociation.Project;
    for (size_t i = 0; i < projectFileTypes.GetCount(); i++) {
        if (ext == projectFileTypes.Item(i)) {
            return FT_PROJECT;
        }
    }

    // see if it's a script file
    wxArrayString& scriptFileTypes = _preferences.General.FileTypeAssociation.Script;
    for (size_t i = 0; i < scriptFileTypes.size(); i++) {
        if (ext == scriptFileTypes.Item(i)) {
            return FT_SCRIPT;
        }
    }

    // see if it's a text file
    wxArrayString& textFileTypes = _preferences.General.FileTypeAssociation.Text;
    for (size_t i = 0; i < textFileTypes.size(); i++) {
        if (ext == textFileTypes.Item(i)) {
            return FT_TEXT;
        }
    }

    return FT_UNKNOWN;
}

//-------------------------------------------------------------------
wxString
Developer::createWildcardString()
{
    wxString wildcard;

    // add "all files" wildcard
    wildcard += "All Files (*.*)|*.*|";

    // add script file types
    wxString scriptWildcard;
    wxArrayString& scriptFileTypes = _preferences.General.FileTypeAssociation.Script;
    for (size_t i = 0; i < scriptFileTypes.GetCount(); i++) {
        scriptWildcard += "*.";
        scriptWildcard += scriptFileTypes.Item(i);
        if (i + 1 < scriptFileTypes.GetCount()) {
            scriptWildcard += ";";
        }
    }
    wildcard += "Script Files (";
    wildcard += scriptWildcard;
    wildcard += ")|";
    wildcard += scriptWildcard;
    wildcard += "|";

    // add text file types
    wxString textWildcard;
    wxArrayString& textFileTypes = _preferences.General.FileTypeAssociation.Text;
    for (size_t i = 0; i < textFileTypes.GetCount(); i++) {
        textWildcard += "*.";
        textWildcard += textFileTypes.Item(i);
        if (i + 1 < textFileTypes.GetCount()) {
            textWildcard += ";";
        }
    }
    wildcard += "Text Files (";
    wildcard += textWildcard;
    wildcard += ")|";
    wildcard += textWildcard;
    wildcard += "|";

    // add project file types
    wxString projectWildcard;
    wxArrayString& projectFileTypes = _preferences.General.FileTypeAssociation.Project;
    for (size_t i = 0; i < projectFileTypes.GetCount(); i++) {
        projectWildcard += "*.";
        projectWildcard += projectFileTypes.Item(i);
        if (i + 1 < projectFileTypes.GetCount()) {
            projectWildcard += ";";
        }
    }
    wildcard += "Project Files (";
    wildcard += projectWildcard;
    wildcard += ")|";
    wildcard += projectWildcard;
    wildcard += "|";

    return wildcard;
}

//-------------------------------------------------------------------
wxString
Developer::createScriptWildcardString()
{
    wxString wildcard;

    // add script file types
    wxString scriptWildcard;
    wxArrayString& scriptFileTypes = _preferences.General.FileTypeAssociation.Script;
    for (size_t i = 0; i < scriptFileTypes.GetCount(); i++) {
        scriptWildcard += "*.";
        scriptWildcard += scriptFileTypes.Item(i);
        if (i + 1 < scriptFileTypes.GetCount()) {
            scriptWildcard += ";";
        }
    }
    wildcard += "Script Files (";
    wildcard += scriptWildcard;
    wildcard += ")|";
    wildcard += scriptWildcard;
    wildcard += "|";

    // add "all files" wildcard
    wildcard += "All Files (*.*)|*.*";

    return wildcard;
}

//-------------------------------------------------------------------
wxString
Developer::createTextWildcardString()
{
    wxString wildcard;

    // add text file types
    wxString textWildcard;
    wxArrayString& textFileTypes = _preferences.General.FileTypeAssociation.Text;
    for (size_t i = 0; i < textFileTypes.GetCount(); i++) {
        textWildcard += "*.";
        textWildcard += textFileTypes.Item(i);
        if (i + 1 < textFileTypes.GetCount()) {
            textWildcard += ";";
        }
    }
    wildcard += "Text Files (";
    wildcard += textWildcard;
    wildcard += ")|";
    wildcard += textWildcard;
    wildcard += "|";

    // add "all files" wildcard
    wildcard += "All Files (*.*)|*.*";

    return wildcard;
}

//-------------------------------------------------------------------
wxString
Developer::createProjectWildcardString()
{
    wxString wildcard;

    // add project file types
    wxString projectWildcard;
    wxArrayString& projectFileTypes = _preferences.General.FileTypeAssociation.Project;
    for (size_t i = 0; i < projectFileTypes.GetCount(); i++) {
        projectWildcard += "*.";
        projectWildcard += projectFileTypes.Item(i);
        if (i + 1 < projectFileTypes.GetCount()) {
            projectWildcard += ";";
        }
    }
    wildcard += "Project Files (";
    wildcard += projectWildcard;
    wildcard += ")|";
    wildcard += projectWildcard;
    wildcard += "|";

    // add "all files" wildcard
    wildcard += "All Files (*.*)|*.*";

    return wildcard;
}

//-------------------------------------------------------------------
void
Developer::updateTitle()
{
    wxString title = wxT("Developer");
    if (_isProjectOpen) {
        if (_projectName.IsEmpty()) {
            title = wxT("Unnamed Project - ") + title;
        } else {
            title = _projectName + wxT(" - ") + title;
        }
    }
    SetTitle(title);
}

//-------------------------------------------------------------------
void
Developer::showFileEditorToolBar(int fileEditorType, bool show)
{
    switch (fileEditorType) {
        case FET_TEXT:
            _windowManager.GetPane("TextToolBar").Show(show);
            break;
        default:
            return;
    }
    _windowManager.Update();
}

//-------------------------------------------------------------------
void
Developer::showProjectDataExplorer(bool show)
{
    _windowManager.GetPane("ProjectDataExplorer").Show(show);
    _windowManager.Update();
}

//-------------------------------------------------------------------
void
Developer::showCommonDataExplorer(bool show)
{
    _windowManager.GetPane("CommonDataExplorer").Show(show);
    _windowManager.Update();
}

//-------------------------------------------------------------------
void
Developer::showOutput(bool show)
{
    _windowManager.GetPane("Output").Show(show);
    _windowManager.Update();
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getStandardToolBarPaneInfo() const
{
    static wxAuiPaneInfo standardToolBarPaneInfo;

    static bool first_call = true;
    if (first_call) {
        standardToolBarPaneInfo.Name("StandardToolBar");
        standardToolBarPaneInfo.Top();
        standardToolBarPaneInfo.ToolbarPane();
        standardToolBarPaneInfo.CloseButton(false);
        first_call = false;
    }

    return standardToolBarPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getPaneToolBarPaneInfo() const
{
    static wxAuiPaneInfo paneToolBarPaneInfo;

    static bool first_call = true;
    if (first_call) {
        paneToolBarPaneInfo.Name("PaneToolBar");
        paneToolBarPaneInfo.Bottom();
        paneToolBarPaneInfo.ToolbarPane();
        paneToolBarPaneInfo.CloseButton(false);
        first_call = false;
    }

    return paneToolBarPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getProjectToolBarPaneInfo() const
{
    static wxAuiPaneInfo projectToolBarPaneInfo;

    static bool first_call = true;
    if (first_call) {
        projectToolBarPaneInfo.Name("ProjectToolBar");
        projectToolBarPaneInfo.Top();
        projectToolBarPaneInfo.ToolbarPane();
        projectToolBarPaneInfo.CloseButton(false);
        first_call = false;
    }

    return projectToolBarPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getTextToolBarPaneInfo() const
{
    static wxAuiPaneInfo textToolBarPaneInfo;

    static bool first_call = true;
    if (first_call) {
        textToolBarPaneInfo.Name("TextToolBar");
        textToolBarPaneInfo.Top();
        textToolBarPaneInfo.ToolbarPane();
        textToolBarPaneInfo.CloseButton(false);
        textToolBarPaneInfo.Hide();
        first_call = false;
    }

    return textToolBarPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getProjectDataExplorerPaneInfo() const
{
    static wxAuiPaneInfo projectDataExplorerPaneInfo;

    static bool first_call = true;
    if (first_call) {
        projectDataExplorerPaneInfo.Name("ProjectDataExplorer");
        projectDataExplorerPaneInfo.Caption("Project Data Explorer");
        projectDataExplorerPaneInfo.Left();
        projectDataExplorerPaneInfo.PinButton();
        projectDataExplorerPaneInfo.MaximizeButton();
        projectDataExplorerPaneInfo.Gripper();
        projectDataExplorerPaneInfo.GripperTop();
        projectDataExplorerPaneInfo.DestroyOnClose(false);
        projectDataExplorerPaneInfo.MinSize(100, 100);
        projectDataExplorerPaneInfo.Hide();
        first_call = false;
    }

    return projectDataExplorerPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getCommonDataExplorerPaneInfo() const
{
    static wxAuiPaneInfo commonDataExplorerPaneInfo;

    static bool first_call = true;
    if (first_call) {
        commonDataExplorerPaneInfo.Name("CommonDataExplorer");
        commonDataExplorerPaneInfo.Caption("Common Data Explorer");
        commonDataExplorerPaneInfo.Left();
        commonDataExplorerPaneInfo.PinButton();
        commonDataExplorerPaneInfo.MaximizeButton();
        commonDataExplorerPaneInfo.Gripper();
        commonDataExplorerPaneInfo.GripperTop();
        commonDataExplorerPaneInfo.DestroyOnClose(false);
        commonDataExplorerPaneInfo.MinSize(100, 100);
        first_call = false;
    }

    return commonDataExplorerPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getOutputPaneInfo() const
{
    static wxAuiPaneInfo outputPaneInfo;

    static bool first_call = true;
    if (first_call) {
        outputPaneInfo.Name("Output");
        outputPaneInfo.Caption("Output");
        outputPaneInfo.Bottom();
        outputPaneInfo.Floatable();
        outputPaneInfo.Dockable();
        outputPaneInfo.PinButton();
        outputPaneInfo.MaximizeButton();
        outputPaneInfo.Gripper();
        outputPaneInfo.GripperTop();
        outputPaneInfo.DestroyOnClose(false);
        outputPaneInfo.MinSize(100, 100);
        outputPaneInfo.Hide();
        first_call = false;
    }

    return outputPaneInfo;
}

//-------------------------------------------------------------------
const wxAuiPaneInfo&
Developer::getFileEditorContainerPaneInfo() const
{
    static wxAuiPaneInfo fileEditorContainerPaneInfo;

    static bool first_call = true;
    if (first_call) {
        fileEditorContainerPaneInfo.Name("FileEditorContainer");
        fileEditorContainerPaneInfo.CenterPane();
        fileEditorContainerPaneInfo.MaximizeButton();
        fileEditorContainerPaneInfo.CloseButton(false);
        first_call = false;
    }

    return fileEditorContainerPaneInfo;
}

//-------------------------------------------------------------------
void
Developer::onClose(wxCloseEvent& event)
{
    // if a project is open, close it
    if (_isProjectOpen) {
        if (!closeProject()) {
            event.Veto();
            return;
        }
    } else {
        // if no project is open, there still could be files open
        if (_editorContainer->GetPageCount() > 0) {
            // see if any files are modified and add them to the item list
            wxArrayString itemList;

            size_t numEditors = _editorContainer->GetPageCount();
            for (size_t i = 0; i < numEditors; i++) {
                wxWindow* fileEditor = _editorContainer->GetPage(i);
                assert(fileEditor);

                IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
                assert(delegator);

                if (delegator->isFileModified()) {
                    itemList.Add(wxFileName(delegator->getFileName()).GetFullName());
                }
            }

            // if there are any modified files, ask if we should save the changes
            if (itemList.GetCount() > 0) {
                // show save changes dialog
                SaveChangesDialog scd(itemList);
                int result = scd.ShowModal();

                if (result == wxID_YES) {
                    // save the changes
                    for (size_t i = 0; i < numEditors; i++) {
                        wxWindow* fileEditor = _editorContainer->GetPage(i);
                        assert(fileEditor);

                        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
                        assert(delegator);

                        if (delegator->isFileModified()) {
                            delegator->saveFile();
                        }
                    }
                }

                if (result == wxID_CANCEL) {
                    event.Veto();
                    return;
                }
            }

            // close all files
            _editorContainer->DeleteAllPages();
            showFileEditorToolBar(FET_TEXT, false);
        }
    }

    _preferences.General.DockManagerState = _windowManager.SavePerspective();

    _preferences.General.Window.Maximized = IsMaximized();

    if (!IsMaximized()) {
        _preferences.General.Window.Width  = GetSize().GetWidth();
        _preferences.General.Window.Height = GetSize().GetHeight();
        _preferences.General.Window.X      = GetPosition().x;
        _preferences.General.Window.Y      = GetPosition().y;
    }

    // save preferences
    _preferences.saveToFile("system/developer/developer.prefs");

    event.Skip();
}

//-------------------------------------------------------------------
void
Developer::onExitDeveloper(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//-------------------------------------------------------------------
void
Developer::onShowAboutDeveloper(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo aboutInfo;

    aboutInfo.SetName("GameGears Developer");
    aboutInfo.SetVersion(DEVELOPER_VERSION_STRING);
    aboutInfo.SetCopyright(wxT("\u00A9 2012 Anatoli Steinmark"));
    aboutInfo.AddDeveloper("Anatoli Steinmark");

    wxAboutBox(aboutInfo);
}

//-------------------------------------------------------------------
void
Developer::onEditDeveloperPreferences(wxCommandEvent& event)
{
    PreferencesDialog pd(_preferences, _initialPath);
    if (pd.ShowModal() == wxID_OK) {
        _preferences = pd.getPreferences();
    }
}

//-------------------------------------------------------------------
void
Developer::onCreateNewProject(wxCommandEvent& event)
{
    createNewProject();
}

//-------------------------------------------------------------------
void
Developer::onOpenProject(wxCommandEvent& event)
{
    wxString defaultDir = _preferences.General.DataLocation;

    // show open project dialog
    wxFileDialog opd(this, wxT("Open Project"), defaultDir, "", createProjectWildcardString(), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (opd.ShowModal() == wxID_CANCEL) {
        return;
    }

    wxString projectFile = opd.GetPath();

    openProject(projectFile);
}


//-------------------------------------------------------------------
void
Developer::onOpenRecentProject(wxCommandEvent& event)
{
    wxArrayString& recentProjects = _preferences.General.RecentProjects;
    size_t index = (size_t)(event.GetId() - CMD_OPEN_RECENT_PROJECT_1);

    if (index < recentProjects.GetCount()) {
        wxString recentProject = recentProjects.Item(index);

        openProject(recentProject);
    }
}

//-------------------------------------------------------------------
void
Developer::onProjectAddNewFile(wxCommandEvent& event)
{
    addNewFile(_projectLocation);
}

//-------------------------------------------------------------------
void
Developer::onStartProject(wxCommandEvent& event)
{

}

//-------------------------------------------------------------------
void
Developer::onProjectFullScreen(wxCommandEvent& event)
{
    _projectFullScreen = !_projectFullScreen;
}

//-------------------------------------------------------------------
void
Developer::onEditProjectProperties(wxCommandEvent& event)
{
    if (!_isProjectOpen) {
        return;
    }

    ProjectProperties projectProperties;
    wxString projectFile = _projectLocation + wxFileName::GetPathSeparator() + "game.ggproj";

    // load properties
    if (!projectProperties.loadFromFile(projectFile.ToStdString())) {
        wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        ed.SetMessage(wxT("Could not load project file '") + projectFile + wxT("'."));
        ed.ShowModal();
        return;
    }

    // edit properties
    ProjectPropertiesDialog ppd(projectProperties);
    if (ppd.ShowModal() == wxID_OK) {
        // user clicked ok, so save the changes
        projectProperties = ppd.getProjectProperties();
        projectProperties.saveToFile(projectFile.ToStdString());

        // if the project name changed, update the internal variable and the title
        if (_projectName != projectProperties.Name) {
            _projectName = projectProperties.Name;
            updateTitle();
        }
    }
}

//-------------------------------------------------------------------
void
Developer::onCloseProject(wxCommandEvent& event)
{
    closeProject();
}

//-------------------------------------------------------------------
void
Developer::onCreateNewFile(wxCommandEvent& event)
{
    createNewFile();
}

//-------------------------------------------------------------------
void
Developer::onOpenFile(wxCommandEvent& event)
{
    wxString defaultDir = (_isProjectOpen ? _projectLocation : _preferences.General.DataLocation);

    // show open file dialog
    wxFileDialog ofd(this, wxT("Open File"), defaultDir, "", createWildcardString(), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (ofd.ShowModal() == wxID_CANCEL) {
        return;
    }

    wxArrayString paths;
    ofd.GetPaths(paths);

    for (size_t i = 0; i < paths.GetCount(); i++) {
        openFile(paths.Item(i));
    }
}

//-------------------------------------------------------------------
void
Developer::onOpenRecentFile(wxCommandEvent& event)
{
    wxArrayString& recentFiles = _preferences.General.RecentFiles;
    size_t index = (size_t)(event.GetId() - CMD_OPEN_RECENT_FILE_1);

    if (index < recentFiles.GetCount()) {
        wxString recentFile = recentFiles.Item(index);

        openFile(recentFile);
    }
}

//-------------------------------------------------------------------
void
Developer::onSaveFile(wxCommandEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        wxWindow* fileEditor = _editorContainer->GetCurrentPage();
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        delegator->saveFile();
    }
}

//-------------------------------------------------------------------
void
Developer::onSaveFileAs(wxCommandEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        wxWindow* fileEditor = _editorContainer->GetCurrentPage();
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        delegator->saveFileAs();
    }
}

//-------------------------------------------------------------------
void
Developer::onSaveAllFiles(wxCommandEvent& event)
{
    size_t numEditors = _editorContainer->GetPageCount();
    for (size_t i = 0; i < numEditors; i++) {
        wxWindow* fileEditor = _editorContainer->GetPage(i);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        if (delegator->isFileModified()) {
            delegator->saveFile();
        }
    }
}

//-------------------------------------------------------------------
void
Developer::onCloseFile(wxCommandEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        wxWindow* fileEditor = _editorContainer->GetCurrentPage();
        assert(fileEditor);

        // see if the file is modified
        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        if (delegator->isFileModified()) {
            // create item list
            wxArrayString itemList;
            itemList.Add(wxFileName(delegator->getFileName()).GetFullName());

            // show save changes dialog
            SaveChangesDialog scd(itemList);
            int result = scd.ShowModal();

            if (result == wxID_YES) {
                // save the changes
                delegator->saveFile();
            }

            if (result == wxID_CANCEL) {
                return;
            }
        }

        // close file
        _editorContainer->DeletePage(_editorContainer->GetSelection());
    }
}

//-------------------------------------------------------------------
void
Developer::onCloseAllFiles(wxCommandEvent& event)
{
    // if there are no files open, nothing to do here
    if (_editorContainer->GetPageCount() == 0) {
        return;
    }

    // see if any files are modified and add them to the item list
    wxArrayString itemList;

    size_t numEditors = _editorContainer->GetPageCount();
    for (size_t i = 0; i < numEditors; i++) {
        wxWindow* fileEditor = _editorContainer->GetPage(i);
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        if (delegator->isFileModified()) {
            itemList.Add(wxFileName(delegator->getFileName()).GetFullName());
        }
    }

    // if there are any modified files, ask if we should save the changes
    if (itemList.GetCount() > 0) {
        // show save changes dialog
        SaveChangesDialog scd(itemList);
        int result = scd.ShowModal();

        if (result == wxID_YES) {
            // save the changes
            for (size_t i = 0; i < numEditors; i++) {
                wxWindow* fileEditor = _editorContainer->GetPage(i);
                assert(fileEditor);

                IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
                assert(delegator);

                if (delegator->isFileModified()) {
                    delegator->saveFile();
                }
            }
        }

        if (result == wxID_CANCEL) {
            // cancel
            return;
        }
    }

    // close all files
    _editorContainer->DeleteAllPages();
    showFileEditorToolBar(FET_TEXT, false);
}

//-------------------------------------------------------------------
void
Developer::onShowProjectDataExplorer(wxCommandEvent& event)
{
    showProjectDataExplorer(!_windowManager.GetPane("ProjectDataExplorer").IsShown());
}

//-------------------------------------------------------------------
void
Developer::onShowCommonDataExplorer(wxCommandEvent& event)
{
    showCommonDataExplorer(!_windowManager.GetPane("CommonDataExplorer").IsShown());
}

//-------------------------------------------------------------------
void
Developer::onShowOutput(wxCommandEvent& event)
{
    showOutput(!_windowManager.GetPane("Output").IsShown());
}

//-------------------------------------------------------------------
void
Developer::onFileEditorCommand(wxCommandEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        wxWindow* fileEditor = _editorContainer->GetCurrentPage();
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        delegator->doCommand(event.GetId());
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_ShowProjectDataExplorer(wxUpdateUIEvent& event)
{
    if (_isProjectOpen) {
        event.Enable(true);
        if (_windowManager.GetPane("ProjectDataExplorer").IsShown()) {
            event.Check(true);
        } else {
            event.Check(false);
        }
    } else {
        event.Check(false);
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_ShowCommonDataExplorer(wxUpdateUIEvent& event)
{
    if (_windowManager.GetPane("CommonDataExplorer").IsShown()) {
        event.Check(true);
    } else {
        event.Check(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_ShowOutput(wxUpdateUIEvent& event)
{
    if (_windowManager.GetPane("Output").IsShown()) {
        event.Check(true);
    } else {
        event.Check(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_ProjectAddNewFile(wxUpdateUIEvent& event)
{
    if (_isProjectOpen) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_StartProject(wxUpdateUIEvent& event)
{
    if (_isProjectOpen) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_ProjectFullScreen(wxUpdateUIEvent& event)
{
    // update check state
    if (_projectFullScreen) {
        event.Check(true);
    } else {
        event.Check(false);
    }

    // update enable state
    if (_isProjectOpen) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_EditProjectProperties(wxUpdateUIEvent& event)
{
    if (_isProjectOpen) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_CloseProject(wxUpdateUIEvent& event)
{
    if (_isProjectOpen) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_SaveFile(wxUpdateUIEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        wxWindow* fileEditor = _editorContainer->GetCurrentPage();
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        if (delegator->isFileModified()) {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_SaveFileAs(wxUpdateUIEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_SaveAllFiles(wxUpdateUIEvent& event)
{
    size_t numEditors = _editorContainer->GetPageCount();
    for (size_t i = 0; i < numEditors; i++) {
        wxWindow* fileEditor = _editorContainer->GetPage(i);
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        if (delegator->isFileModified()) {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_CloseFile(wxUpdateUIEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_CloseAllFiles(wxUpdateUIEvent& event)
{
    if (_editorContainer->GetPageCount() > 0) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}

//-------------------------------------------------------------------
void
Developer::onUpdateUI_FileEditorCommand(wxUpdateUIEvent& event)
{
    if (_editorContainer->GetPageCount() > 0 && _editorContainer->GetSelection() != wxNOT_FOUND) {
        wxWindow* fileEditor = _editorContainer->GetCurrentPage();
        assert(fileEditor);

        IFileEditorDelegator* delegator = (IFileEditorDelegator*)(fileEditor->GetClientData());
        assert(delegator);

        if (delegator->canDoCommand(event.GetId())) {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
}

//-------------------------------------------------------------------
void
Developer::onFileEditorSelectionChanged(wxAuiNotebookEvent& event)
{
    int oldSelectionPageIndex = event.GetOldSelection();
    int oldFileEditorType     = FET_UNKNOWN;

    int newSelectionPageIndex = event.GetSelection();
    int newFileEditorType     = FET_UNKNOWN;

    // get old editor type
    if (oldSelectionPageIndex != wxNOT_FOUND) {
        wxWindow* oldFileEditor = _editorContainer->GetPage(oldSelectionPageIndex);
        assert(oldFileEditor);

        IFileEditorDelegator* oldFileEditorDelegator = (IFileEditorDelegator*)(oldFileEditor->GetClientData());
        assert(oldFileEditorDelegator);

        oldFileEditorType = oldFileEditorDelegator->getFileEditorType();
        assert(oldFileEditorType != FET_UNKNOWN);
    }

    // get new editor type
    wxWindow* newFileEditor = _editorContainer->GetPage(newSelectionPageIndex);
    assert(newFileEditor);

    IFileEditorDelegator* newFileEditorDelegator = (IFileEditorDelegator*)(newFileEditor->GetClientData());
    assert(newFileEditorDelegator);

    newFileEditorType = newFileEditorDelegator->getFileEditorType();
    assert(newFileEditorType != FET_UNKNOWN);

    if (oldFileEditorType != FET_UNKNOWN && oldFileEditorType != newFileEditorType) {
        // hide the old tool bar
        showFileEditorToolBar(oldFileEditorType, false);

        // remove the old editor menu
        _menuBar->removeFileEditorMenu();
    }

    if (oldFileEditorType == FET_UNKNOWN || oldFileEditorType != newFileEditorType) {
        // show the new tool bar
        showFileEditorToolBar(newFileEditorType);

        // set the new editor menu
        _menuBar->setFileEditorMenu(newFileEditorType);
    }
}

//-------------------------------------------------------------------
void
Developer::onFileEditorClose(wxAuiNotebookEvent& event)
{
    int pageIndex = event.GetSelection();

    wxWindow* editor = _editorContainer->GetPage(pageIndex);
    assert(editor);

    IFileEditorDelegator* delegator = (IFileEditorDelegator*)(editor->GetClientData());
    assert(delegator);

    // see if file is modified
    if (delegator->isFileModified()) {
        // create item list
        wxArrayString itemList;
        itemList.Add(wxFileName(delegator->getFileName()).GetFullName());

        // show save changes dialog
        SaveChangesDialog scd(itemList);
        int result = scd.ShowModal();

        if (result == wxID_YES) {
            // save the changes
            delegator->saveFile();
        }

        if (result == wxID_CANCEL) {
            // don't close
            event.Veto();
            return;
        }
    }

    if (_editorContainer->GetPageCount() == 1) { // the last editor is about to be closed
        int fileEditorType = delegator->getFileEditorType();
        assert(fileEditorType != FET_UNKNOWN);

        // hide the editor tool bar
        showFileEditorToolBar(fileEditorType, false);

        // remove the editor menu
        _menuBar->removeFileEditorMenu();
    }
}
