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

#include <sstream>
#include "MenuBar.hpp"
#include "Developer.hpp"
#include "commands.hpp"

#define MENUBAR_STYLE (0)


//-------------------------------------------------------------------
MenuBar::MenuBar(Developer* developer)
    : wxMenuBar(MENUBAR_STYLE)
    , _developer(developer)
    , _fileEditorType(FET_UNKNOWN)
{
    Append(createFileMenu(),    _("&File"));
    Append(createEditMenu(),    _("&Edit"));
    Append(createViewMenu(),    _("&View"));
    Append(createProjectMenu(), _("&Project"));
    Append(createToolsMenu(),   _("&Tools"));
    Append(createHelpMenu(),    _("&Help"));
}

//-------------------------------------------------------------------
MenuBar::~MenuBar()
{
}

//-------------------------------------------------------------------
void
MenuBar::updateFileMenu()
{
    wxMenu* oldFileMenu = Replace(0, createFileMenu(), _("&File"));
    delete oldFileMenu;
}

//-------------------------------------------------------------------
void
MenuBar::removeFileEditorMenu()
{
    if (_fileEditorType != FET_UNKNOWN) {
        wxMenu* fileEditorMenu = Remove(4);
        delete fileEditorMenu;
        _fileEditorType = FET_UNKNOWN;
    }
}

//-------------------------------------------------------------------
void
MenuBar::setFileEditorMenu(int fileEditorType)
{
    // remove previous editor menu
    removeFileEditorMenu();

    // create the new editor menu
    switch (fileEditorType) {
        case FET_TEXT:
            Insert(4, createTextEditorMenu(), _("&Text"));
            _fileEditorType = fileEditorType;
        break;
    }
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createFileMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    // New
    subMenu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_CREATE_NEW_PROJECT, _("Project\tCtrl+Shift+N"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/project-new.png"), wxBITMAP_TYPE_PNG));
    subMenu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_CREATE_NEW_FILE, _("File\tCtrl+N"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-new.png"), wxBITMAP_TYPE_PNG));
    subMenu->Append(menuItem);

    menu->AppendSubMenu(subMenu, _("New"));

    // Open
    subMenu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_OPEN_PROJECT, _("Project\tCtrl+Shift+O"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/project-open.png"), wxBITMAP_TYPE_PNG));
    subMenu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_OPEN_FILE, _("File\tCtrl+O"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-open.png"), wxBITMAP_TYPE_PNG));
    subMenu->Append(menuItem);

    menu->AppendSubMenu(subMenu, _("Open"));

    // Separator
    menu->AppendSeparator();

    // Close
    menuItem = new wxMenuItem(0, CMD_CLOSE_FILE, _("Close"));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_CLOSE_ALL_FILES, _("Close All"));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_CLOSE_PROJECT, _("Close Project"));
    menu->Append(menuItem);

    // Separator
    menu->AppendSeparator();

    // Save
    menuItem = new wxMenuItem(0, CMD_SAVE_FILE, _("Save\tCtrl+S"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-save.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_SAVE_FILE_AS, _("Save As"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-save-as.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_SAVE_ALL_FILES, _("Save All\tCtrl+Shift+S"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-save-all.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    // Separator
    menu->AppendSeparator();

    // Recent Files
    subMenu = new wxMenu();
    wxMenuItem* recentFilesSubMenuItem = menu->AppendSubMenu(subMenu, _("Recent Files"));

    const wxArrayString& recentFiles = _developer->getPreferences().General.RecentFiles;
    if (recentFiles.GetCount() > 0) {
        for (size_t i = 0; i < 10 && i < recentFiles.size(); i++) {
            wxString prefix;
            prefix << (i + 1) << " ";
            subMenu->Append(CMD_OPEN_RECENT_FILE_1 + i, prefix + recentFiles.Item(i));
        }
    } else {
        recentFilesSubMenuItem->Enable(false);
    }

    // Recent Projects
    subMenu = new wxMenu();
    wxMenuItem* recentProjectsSubMenuItem = menu->AppendSubMenu(subMenu, _("Recent Projects"));

    const wxArrayString& recentProjects = _developer->getPreferences().General.RecentProjects;
    if (recentProjects.GetCount() > 0) {
        for (size_t i = 0; i < 10 && i < recentProjects.size(); i++) {
            wxString prefix;
            prefix << (i + 1) << " ";
            subMenu->Append(CMD_OPEN_RECENT_PROJECT_1 + i, prefix + recentProjects.Item(i));
        }
    } else {
        recentProjectsSubMenuItem->Enable(false);
    }

    // Separator
    menu->AppendSeparator();

    // Exit
    menuItem = new wxMenuItem(0, CMD_EXIT_DEVELOPER, _("Exit\tAlt+F4"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/system-exit.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    return menu;
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createEditMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_UNDO, _("Undo\tCtrl+Z"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/undo.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_REDO, _("Redo\tCtrl+Y"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/redo.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_CUT, _("Cut\tCtrl+X"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/cut.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_COPY, _("Copy\tCtrl+C"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/copy.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_PASTE, _("Paste\tCtrl+V"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/paste.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    return menu;
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createViewMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_SHOW_PROJECT_DATA_EXPLORER, _("Project Data Explorer"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/project-data-explorer.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_SHOW_COMMON_DATA_EXPLORER, _("Common Data Explorer"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/common-data-explorer.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_SHOW_OUTPUT, _("Output"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/output.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    return menu;
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createProjectMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_PROJECT_ADD_NEW_FILE, _("Add New File\tCtrl+Shift+A"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-add-new.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_START_PROJECT, _("Start Project\tF5"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/project-start.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_PROJECT_FULLSCREEN, _("Full Screen\tF6"), wxEmptyString, wxITEM_CHECK);
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_EDIT_PROJECT_PROPERTIES, _("Properties\tF7"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/project-properties.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    return menu;
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createTextEditorMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_TEXT_ZOOM_IN, _("Zoom In\tCtrl+M"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/zoom-in.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_TEXT_ZOOM_OUT, _("Zoom Out\tCtrl+Shift+M"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/zoom-out.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_TEXT_COMMENT_OUT, _("Comment Out\tCtrl+K"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-comment-out.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_TEXT_UNCOMMENT, _("Uncomment\tCtrl+Shift+K"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-uncomment.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_TEXT_INDENT, _("Indent\tCtrl+I"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-indent.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_TEXT_OUTDENT, _("Outdent\tCtrl+Shift+I"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-outdent.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_TEXT_MOVE_UP, _("Move Up\tCtrl+Up"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-move-up.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menuItem = new wxMenuItem(0, CMD_TEXT_MOVE_DOWN, _("Move Down\tCtrl+Down"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-move-down.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    menu->AppendSeparator();

    menuItem = new wxMenuItem(0, CMD_TEXT_GOTO, _("Go To Line\tCtrl+G"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/text-goto.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    return menu;
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createToolsMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_EDIT_DEVELOPER_PREFERENCES, _("Preferences\tAlt+P"));
    menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/preferences.png"), wxBITMAP_TYPE_PNG));
    menu->Append(menuItem);

    return menu;
}

//-------------------------------------------------------------------
wxMenu*
MenuBar::createHelpMenu()
{
    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    menuItem = new wxMenuItem(0, CMD_SHOW_ABOUT_DEVELOPER, _("About Developer\tAlt+A"));
    menu->Append(menuItem);

    return menu;
}
