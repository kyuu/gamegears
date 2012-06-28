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

#ifndef PROJECTPROPERTIESDIALOG_HPP
#define PROJECTPROPERTIESDIALOG_HPP

#include <wx/wx.h>
#include "ProjectProperties.hpp"


class ProjectPropertiesDialog : public wxDialog {
public:
    enum {
        ID_OK = 0,
        ID_CANCEL,
        ID_WINDOW_WIDTH,
        ID_WINDOW_HEIGHT,
        ID_WINDOW_FULL_SCREEN,
        ID_NAME,
        ID_DESCRIPTION,
        ID_ENTRY_POINT,
    };

    explicit ProjectPropertiesDialog(const ProjectProperties& initialProperties);
    ~ProjectPropertiesDialog();

    const ProjectProperties& getProjectProperties() const;

private:
    void onOk(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);

private:
    ProjectProperties _projectProperties;

    // controls
    wxTextCtrl* _windowWidthEntry;
    wxTextCtrl* _windowHeightEntry;
    wxCheckBox* _windowFullScreenCheckBox;

    wxTextCtrl* _nameEntry;
    wxTextCtrl* _descriptionEntry;
    wxTextCtrl* _entryPointEntry;

    wxButton* _okButton;
    wxButton* _cancelButton;
};


#endif
