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

#include "StandardToolBar.hpp"
#include "Developer.hpp"
#include "commands.hpp"

#define STANDARDTOOLBAR_STYLE (wxAUI_TB_GRIPPER)


//-------------------------------------------------------------------
StandardToolBar::StandardToolBar(Developer* developer)
    : wxAuiToolBar(developer, wxID_ANY, wxDefaultPosition, wxDefaultSize, STANDARDTOOLBAR_STYLE)
    , _developer(developer)
{
    AddTool(CMD_CREATE_NEW_PROJECT, wxEmptyString, wxBitmap(wxT("system/developer/icons/project-new.png"),    wxBITMAP_TYPE_PNG), wxT("New Project"));
    AddTool(CMD_CREATE_NEW_FILE,    wxEmptyString, wxBitmap(wxT("system/developer/icons/file-new.png"),       wxBITMAP_TYPE_PNG), wxT("New File"));
    AddTool(CMD_OPEN_PROJECT,       wxEmptyString, wxBitmap(wxT("system/developer/icons/project-open.png"),   wxBITMAP_TYPE_PNG), wxT("Open Project"));
    AddTool(CMD_OPEN_FILE,          wxEmptyString, wxBitmap(wxT("system/developer/icons/file-open.png"),      wxBITMAP_TYPE_PNG), wxT("Open File"));
    AddTool(CMD_SAVE_FILE,          wxEmptyString, wxBitmap(wxT("system/developer/icons/file-save.png"),      wxBITMAP_TYPE_PNG), wxT("Save"));
    AddTool(CMD_SAVE_ALL_FILES,     wxEmptyString, wxBitmap(wxT("system/developer/icons/file-save-all.png"),  wxBITMAP_TYPE_PNG), wxT("Save All"));

    AddSeparator();

    AddTool(CMD_CUT,   wxEmptyString, wxBitmap(wxT("system/developer/icons/cut.png"),   wxBITMAP_TYPE_PNG), wxT("Cut"));
    AddTool(CMD_COPY,  wxEmptyString, wxBitmap(wxT("system/developer/icons/copy.png"),  wxBITMAP_TYPE_PNG), wxT("Copy"));
    AddTool(CMD_PASTE, wxEmptyString, wxBitmap(wxT("system/developer/icons/paste.png"), wxBITMAP_TYPE_PNG), wxT("Paste"));

    AddSeparator();

    AddTool(CMD_UNDO, wxEmptyString, wxBitmap(wxT("system/developer/icons/undo.png"), wxBITMAP_TYPE_PNG), wxT("Undo"));
    AddTool(CMD_REDO, wxEmptyString, wxBitmap(wxT("system/developer/icons/redo.png"), wxBITMAP_TYPE_PNG), wxT("Redo"));

    AddSeparator();

    AddTool(CMD_EDIT_DEVELOPER_PREFERENCES, wxEmptyString, wxBitmap(wxT("system/developer/icons/preferences.png"), wxBITMAP_TYPE_PNG), wxT("Preferences"));
}

//-------------------------------------------------------------------
StandardToolBar::~StandardToolBar()
{
}
