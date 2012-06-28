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

#ifndef TEXTTOOLBAR_HPP
#define TEXTTOOLBAR_HPP

#include <wx/wx.h>
#include <wx/aui/aui.h>


// forward declaration
class Developer;


class TextToolBar : public wxAuiToolBar {
public:
    explicit TextToolBar(Developer* developer);
    ~TextToolBar();

private:
    Developer* _developer;
};


#endif
