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

#include "ProjectToolBar.hpp"
#include "Developer.hpp"
#include "commands.hpp"

#define PROJECTTOOLBAR_STYLE (wxAUI_TB_GRIPPER)


//-------------------------------------------------------------------
ProjectToolBar::ProjectToolBar(Developer* developer)
    : wxAuiToolBar(developer, wxID_ANY, wxDefaultPosition, wxDefaultSize, PROJECTTOOLBAR_STYLE)
    , _developer(developer)
{
    AddTool(CMD_START_PROJECT,      wxEmptyString, wxBitmap(wxT("system/developer/icons/project-start.png"),       wxBITMAP_TYPE_PNG), wxT("Start Project"));
    AddTool(CMD_PROJECT_FULLSCREEN, wxEmptyString, wxBitmap(wxT("system/developer/icons/project-full-screen.png"), wxBITMAP_TYPE_PNG), wxT("Full Screen"), wxITEM_CHECK);

    AddSeparator();

    AddTool(CMD_EDIT_PROJECT_PROPERTIES, wxEmptyString, wxBitmap(wxT("system/developer/icons/project-properties.png"), wxBITMAP_TYPE_PNG), wxT("Project Properties"));
}

//-------------------------------------------------------------------
ProjectToolBar::~ProjectToolBar()
{
}
