#ifndef DEVELOPER_HPP
#define DEVELOPER_HPP

#include <string>
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "Preferences.hpp"


// forward declaration
class FileExplorer;


enum {
    ID_NEW_PROJECT = 0,
    ID_NEW_CONTENT,
    ID_OPEN_PROJECT,
    ID_OPEN_FILE,
    ID_CLOSE,
    ID_CLOSE_PROJECT,
    ID_SAVE,
    ID_SAVE_AS,
    ID_SAVE_ALL,
    ID_CUT,
    ID_COPY,
    ID_PASTE,
    ID_UNDO,
    ID_REDO,
    ID_DEBUG,
    ID_ZOOM_IN,
    ID_ZOOM_OUT,

    ID_QUIT,
    ID_ABOUT,

    ID_PROJECT_DATA_EXPLORER,
    ID_COMMON_DATA_EXPLORER,
    ID_STANDARD_TOOLBAR,

    ID_PROJECT_PROPERTIES,
    ID_PREFERENCES,

    ID_RECENT_FILE_0,
    ID_RECENT_FILE_1,
    ID_RECENT_FILE_2,
    ID_RECENT_FILE_3,
    ID_RECENT_FILE_4,
    ID_RECENT_FILE_5,
    ID_RECENT_FILE_6,
    ID_RECENT_FILE_7,
    ID_RECENT_FILE_8,
    ID_RECENT_FILE_9,

    ID_RECENT_PROJECT_0,
    ID_RECENT_PROJECT_1,
    ID_RECENT_PROJECT_2,
    ID_RECENT_PROJECT_3,
    ID_RECENT_PROJECT_4,
    ID_RECENT_PROJECT_5,
    ID_RECENT_PROJECT_6,
    ID_RECENT_PROJECT_7,
    ID_RECENT_PROJECT_8,
    ID_RECENT_PROJECT_9,
};


class Developer : public wxFrame {
public:
    Developer();
    ~Developer();

    Preferences& getPreferences();

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnViewProjectDataExplorer(wxCommandEvent& event);
    void OnViewCommonDataExplorer(wxCommandEvent& event);
    void OnViewStandardToolbar(wxCommandEvent& event);

    void onEditorCommand(wxCommandEvent& e);
    void onCommandUpdateUI(wxUpdateUIEvent& e);
    void onViewUpdateUI(wxUpdateUIEvent& e);

private:
    Preferences    _prefs;
    wxAuiManager   _windowManager;
    wxAuiNotebook* _documentContainer;
    FileExplorer*  _projectDataExplorer;
    FileExplorer*  _commonDataExplorer;
    wxAuiToolBar*  _toolbar;
    wxMenuBar*     _menubar;
    wxStatusBar*   _statusbar;

    struct {
        std::string PathTo;

        std::string Title;
        std::string Description;

        struct {
            int  Width;
            int  Height;
        } Window;

        std::string EntryPoint;

    } _project;

};


#endif
