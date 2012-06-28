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

#include "PaneToolBar.hpp"
#include "Developer.hpp"
#include "commands.hpp"

#define PANETOOLBAR_STYLE (wxAUI_TB_GRIPPER)


//-------------------------------------------------------------------
PaneToolBar::PaneToolBar(Developer* developer)
    : wxAuiToolBar(developer, wxID_ANY, wxDefaultPosition, wxDefaultSize, PANETOOLBAR_STYLE)
    , _developer(developer)
{
    AddTool(CMD_SHOW_PROJECT_DATA_EXPLORER, wxEmptyString, wxBitmap(wxT("system/developer/icons/project-data-explorer.png"), wxBITMAP_TYPE_PNG), wxT("Project Data Explorer"), wxITEM_CHECK);
    AddTool(CMD_SHOW_COMMON_DATA_EXPLORER,  wxEmptyString, wxBitmap(wxT("system/developer/icons/common-data-explorer.png"),  wxBITMAP_TYPE_PNG), wxT("Common Data Explorer"),  wxITEM_CHECK);
    AddTool(CMD_SHOW_OUTPUT,                wxEmptyString, wxBitmap(wxT("system/developer/icons/output.png"),                wxBITMAP_TYPE_PNG), wxT("Output"),                wxITEM_CHECK);
}

//-------------------------------------------------------------------
PaneToolBar::~PaneToolBar()
{
}
