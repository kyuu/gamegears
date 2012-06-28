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

#include "TextToolBar.hpp"
#include "Developer.hpp"
#include "commands.hpp"

#define TEXTTOOLBAR_STYLE (wxAUI_TB_GRIPPER)


//-------------------------------------------------------------------
TextToolBar::TextToolBar(Developer* developer)
    : wxAuiToolBar(developer, wxID_ANY, wxDefaultPosition, wxDefaultSize, TEXTTOOLBAR_STYLE)
    , _developer(developer)
{
    AddTool(CMD_TEXT_ZOOM_IN,  wxEmptyString, wxBitmap(wxT("system/developer/icons/zoom-in.png"),  wxBITMAP_TYPE_PNG), wxT("Zoom In"));
    AddTool(CMD_TEXT_ZOOM_OUT, wxEmptyString, wxBitmap(wxT("system/developer/icons/zoom-out.png"), wxBITMAP_TYPE_PNG), wxT("Zoom Out"));

    AddSeparator();

    AddTool(CMD_TEXT_COMMENT_OUT, wxEmptyString, wxBitmap(wxT("system/developer/icons/text-comment-out.png"), wxBITMAP_TYPE_PNG), wxT("Comment Out Selected Lines"));
    AddTool(CMD_TEXT_UNCOMMENT,   wxEmptyString, wxBitmap(wxT("system/developer/icons/text-uncomment.png"),   wxBITMAP_TYPE_PNG), wxT("Uncomment Selected Lines"));

    AddSeparator();

    AddTool(CMD_TEXT_INDENT,  wxEmptyString, wxBitmap(wxT("system/developer/icons/text-indent.png"),  wxBITMAP_TYPE_PNG), wxT("Indent Selected Lines"));
    AddTool(CMD_TEXT_OUTDENT, wxEmptyString, wxBitmap(wxT("system/developer/icons/text-outdent.png"), wxBITMAP_TYPE_PNG), wxT("Outdent Selected Lines"));

    AddSeparator();

    AddTool(CMD_TEXT_MOVE_UP,   wxEmptyString, wxBitmap(wxT("system/developer/icons/text-move-up.png"),   wxBITMAP_TYPE_PNG), wxT("Move Selected Lines Up"));
    AddTool(CMD_TEXT_MOVE_DOWN, wxEmptyString, wxBitmap(wxT("system/developer/icons/text-move-down.png"), wxBITMAP_TYPE_PNG), wxT("Move Selected Lines Down"));

    AddSeparator();

    AddTool(CMD_TEXT_GOTO, wxEmptyString, wxBitmap(wxT("system/developer/icons/text-goto.png"), wxBITMAP_TYPE_PNG), wxT("Go To Line"));
}

//-------------------------------------------------------------------
TextToolBar::~TextToolBar()
{
}
