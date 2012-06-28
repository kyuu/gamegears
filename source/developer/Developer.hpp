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

#ifndef DEVELOPER_HPP
#define DEVELOPER_HPP

#include <string>
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "Preferences.hpp"


enum {
    FT_UNKNOWN = 0,
    FT_DIRECTORY,
    FT_PROJECT,
    FT_SCRIPT,
    FT_TEXT,
};

enum {
    FET_UNKNOWN = 0,
    FET_TEXT,
};


// forward declaration
class StatusBar;
class MenuBar;
class StandardToolBar;
class PaneToolBar;
class ProjectToolBar;
class TextToolBar;
class Output;
class FileExplorer;


class Developer : public wxFrame {
public:
    Developer();
    ~Developer();

    Preferences& getPreferences();

    bool isProjectOpen() const;
    const wxString& getProjectLocation() const;
    bool createNewProject();
    bool openProject(const wxString& projectFile);
    bool closeProject();

    bool createNewFile();
    bool addNewFile(const wxString& defaultLocation);
    bool openFile(const wxString& fileName);
    bool openFileAs(const wxString& fileName, int fileType);

    // TODO: Should these really be methods of this class?
    int getFileType(const wxString& fileName);
    wxString createWildcardString();
    wxString createProjectWildcardString();
    wxString createScriptWildcardString();
    wxString createTextWildcardString();

    // invoked by file editors
    void fileEditorSavePointReached(wxWindow* fileEditor);
    void fileEditorSavePointLeft(wxWindow* fileEditor);
    void fileEditorFileSaved(wxWindow* fileEditor);

private:
    void updateTitle();
    void showFileEditorToolBar(int fileEditorType, bool show = true);
    void showProjectDataExplorer(bool show = true);
    void showCommonDataExplorer(bool show = true);
    void showOutput(bool show = true);

    // TODO: Do we really need these methods?
    const wxAuiPaneInfo& getStandardToolBarPaneInfo() const;
    const wxAuiPaneInfo& getPaneToolBarPaneInfo() const;
    const wxAuiPaneInfo& getProjectToolBarPaneInfo() const;
    const wxAuiPaneInfo& getTextToolBarPaneInfo() const;
    const wxAuiPaneInfo& getOutputPaneInfo() const;
    const wxAuiPaneInfo& getProjectDataExplorerPaneInfo() const;
    const wxAuiPaneInfo& getCommonDataExplorerPaneInfo() const;
    const wxAuiPaneInfo& getFileEditorContainerPaneInfo() const;

    // command event handlers
    void onClose(wxCloseEvent& event);

    void onExitDeveloper(wxCommandEvent& event);
    void onShowAboutDeveloper(wxCommandEvent& event);
    void onEditDeveloperPreferences(wxCommandEvent& event);

    void onShowOutput(wxCommandEvent& event);
    void onShowProjectDataExplorer(wxCommandEvent& event);
    void onShowCommonDataExplorer(wxCommandEvent& event);

    void onCreateNewProject(wxCommandEvent& event);
    void onOpenProject(wxCommandEvent& event);
    void onOpenRecentProject(wxCommandEvent& event);
    void onProjectAddNewFile(wxCommandEvent& event);
    void onStartProject(wxCommandEvent& event);
    void onProjectFullScreen(wxCommandEvent& event);
    void onEditProjectProperties(wxCommandEvent& event);
    void onCloseProject(wxCommandEvent& event);

    void onCreateNewFile(wxCommandEvent& event);
    void onOpenFile(wxCommandEvent& event);
    void onOpenRecentFile(wxCommandEvent& event);
    void onSaveFile(wxCommandEvent& event);
    void onSaveFileAs(wxCommandEvent& event);
    void onSaveAllFiles(wxCommandEvent& event);
    void onCloseFile(wxCommandEvent& event);
    void onCloseAllFiles(wxCommandEvent& event);

    void onFileEditorCommand(wxCommandEvent& event);

    // update UI event handlers
    void onUpdateUI_ShowProjectDataExplorer(wxUpdateUIEvent& event);
    void onUpdateUI_ShowCommonDataExplorer(wxUpdateUIEvent& event);
    void onUpdateUI_ShowOutput(wxUpdateUIEvent& event);

    void onUpdateUI_ProjectAddNewFile(wxUpdateUIEvent& event);
    void onUpdateUI_StartProject(wxUpdateUIEvent& event);
    void onUpdateUI_ProjectFullScreen(wxUpdateUIEvent& event);
    void onUpdateUI_EditProjectProperties(wxUpdateUIEvent& event);
    void onUpdateUI_CloseProject(wxUpdateUIEvent& event);

    void onUpdateUI_SaveFile(wxUpdateUIEvent& event);
    void onUpdateUI_SaveFileAs(wxUpdateUIEvent& event);
    void onUpdateUI_SaveAllFiles(wxUpdateUIEvent& event);
    void onUpdateUI_CloseFile(wxUpdateUIEvent& event);
    void onUpdateUI_CloseAllFiles(wxUpdateUIEvent& event);

    void onUpdateUI_FileEditorCommand(wxUpdateUIEvent& event);

    // AUI notebook event handlers
    void onFileEditorSelectionChanged(wxAuiNotebookEvent& event);
    void onFileEditorClose(wxAuiNotebookEvent& event);

private:
    wxString _initialPath;

    Preferences    _preferences;
    wxAuiManager   _windowManager;
    wxAuiNotebook* _editorContainer;

    // status bar
    StatusBar* _statusBar;

    // menu bar
    MenuBar* _menuBar;

    // tool bars
    StandardToolBar* _standardToolBar;
    PaneToolBar*     _paneToolBar;
    ProjectToolBar*  _projectToolBar;
    TextToolBar*     _textToolBar;

    // file explorers
    FileExplorer* _projectDataExplorer;
    FileExplorer* _commonDataExplorer;

    // output
    Output* _output;

    bool     _isProjectOpen;
    wxString _projectName;
    wxString _projectLocation;
    bool     _projectFullScreen;

};


#endif
