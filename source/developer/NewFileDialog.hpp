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

#ifndef NEWFILEDIALOG_HPP
#define NEWFILEDIALOG_HPP

#include <wx/wx.h>
#include <wx/listctrl.h>


class NewFileDialog : public wxDialog {
public:
    enum {
        ID_FT_SCRIPT = 0,
        ID_FT_TEXT,
    };

    NewFileDialog();
    ~NewFileDialog();

    int getSelectedFileType() const;

private:
    void onFileTypeSelected(wxListEvent& event);
    void onFileTypeDeselected(wxListEvent& event);
    void onFileTypeActivated(wxListEvent& event);

    void onOk(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);

    void onUpdateUI_Ok(wxUpdateUIEvent& event);

private:
    int _selectedFileType;
};


#endif
