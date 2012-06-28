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

#include "NewProjectDialog.hpp"

#define NEWPROJECTDIALOG_STYLE (wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU)

#define MAX_WINDOW_WIDTH  4096
#define MAX_WINDOW_HEIGHT 4096


//-------------------------------------------------------------------
NewProjectDialog::NewProjectDialog(const wxString& defaultLocation)
    : wxDialog(0, wxID_ANY, wxT("New Project"), wxDefaultPosition, wxSize(500, 400), NEWPROJECTDIALOG_STYLE)
{
    SetMinSize(wxSize(500, 400));

    // create box 1
    wxStaticBoxSizer* box1 = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Window Properties"));

    wxFlexGridSizer* fgs1 = new wxFlexGridSizer(2, 2, 5, 5);

    wxStaticText* windowWidthLabel  = new wxStaticText(box1->GetStaticBox(), wxID_ANY, wxT("Width:"));
    wxStaticText* windowHeightLabel = new wxStaticText(box1->GetStaticBox(), wxID_ANY, wxT("Height:"));

    _windowWidthEntry  = new wxTextCtrl(box1->GetStaticBox(), ID_WINDOW_WIDTH,  wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE);
    _windowHeightEntry = new wxTextCtrl(box1->GetStaticBox(), ID_WINDOW_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE);

    // limit window dimensions to 4 digits
    _windowWidthEntry->SetMaxLength(4);
    _windowHeightEntry->SetMaxLength(4);

    fgs1->Add(windowWidthLabel);
    fgs1->Add(_windowWidthEntry);

    fgs1->Add(windowHeightLabel);
    fgs1->Add(_windowHeightEntry);

    _windowFullScreenCheckBox = new wxCheckBox(box1->GetStaticBox(), ID_WINDOW_FULL_SCREEN, wxT("Full Screen"));

    box1->Add(fgs1, 0, wxALL, 6);
    box1->Add(_windowFullScreenCheckBox, 0, wxALL, 6);

    // create box 2
    wxFlexGridSizer* box2 = new wxFlexGridSizer(4, 3, 5, 5);

    wxStaticText* nameLabel        = new wxStaticText(this, wxID_ANY, wxT("Name:"));
    wxStaticText* descriptionLabel = new wxStaticText(this, wxID_ANY, wxT("Description:"));
    wxStaticText* entryPointLabel  = new wxStaticText(this, wxID_ANY, wxT("Entry Point:"));
    wxStaticText* locationLabel    = new wxStaticText(this, wxID_ANY, wxT("Location:"));

    _nameEntry        = new wxTextCtrl(this, ID_NAME);
    _descriptionEntry = new wxTextCtrl(this, ID_DESCRIPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_DONTWRAP);
    _entryPointEntry  = new wxTextCtrl(this, ID_ENTRY_POINT);
    _locationEntry    = new wxTextCtrl(this, ID_LOCATION, defaultLocation);

    _locationPicker = new wxDirPickerCtrl(this, ID_BROWSE_LOCATION, defaultLocation, wxT("Project Location"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST);

    box2->Add(nameLabel);
    box2->Add(_nameEntry, 1, wxEXPAND);
    box2->Add(new wxStaticText(this, wxID_ANY, wxT("")));

    box2->Add(descriptionLabel);
    box2->Add(_descriptionEntry, 1, wxEXPAND);
    box2->Add(new wxStaticText(this, wxID_ANY, wxT("")));

    box2->Add(entryPointLabel);
    box2->Add(_entryPointEntry, 1, wxEXPAND);
    box2->Add(new wxStaticText(this, wxID_ANY, wxT("")));

    box2->Add(locationLabel);
    box2->Add(_locationEntry, 1, wxEXPAND);
    box2->Add(_locationPicker);

    box2->AddGrowableRow(1, 1);
    box2->AddGrowableCol(1, 1);

    // create box 3
    wxBoxSizer* box3 = new wxBoxSizer(wxHORIZONTAL);

    _okButton     = new wxButton(this, ID_OK,     wxT("OK"));
    _cancelButton = new wxButton(this, ID_CANCEL, wxT("Cancel"));

    box3->Add(_okButton);
    box3->AddSpacer(5);
    box3->Add(_cancelButton);

    // create main sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(box1, 0, wxEXPAND      | wxALL, 10);
    sizer->Add(box2, 1, wxEXPAND      | wxALL, 10);
    sizer->Add(box3, 0, wxALIGN_RIGHT | wxALL, 10);

    // apply sizer
    SetSizer(sizer);

    // initialize control values
    _nameEntry->SetValue(_projectProperties.Name);
    _descriptionEntry->SetValue(_projectProperties.Description);
    _entryPointEntry->SetValue(_projectProperties.EntryPoint);

    wxString windowHeightStr;
    windowHeightStr << _projectProperties.Window.Height;
    _windowHeightEntry->SetValue(windowHeightStr);

    wxString windowWidthStr;
    windowWidthStr << _projectProperties.Window.Width;
    _windowWidthEntry->SetValue(windowWidthStr);

    _windowFullScreenCheckBox->SetValue(_projectProperties.Window.FullScreen);

    // set icon
    SetIcon(wxIcon(wxT("system/developer/icons/project-new.png"), wxBITMAP_TYPE_PNG));

    // connect events
    Connect(ID_OK,     wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewProjectDialog::onOk));
    Connect(ID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewProjectDialog::onCancel));

    Connect(ID_BROWSE_LOCATION, wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(NewProjectDialog::onLocationPicked));
}

//-------------------------------------------------------------------
NewProjectDialog::~NewProjectDialog()
{
}

//-------------------------------------------------------------------
const ProjectProperties&
NewProjectDialog::getProjectProperties() const
{
    return _projectProperties;
}

//-------------------------------------------------------------------
const wxString&
NewProjectDialog::getProjectLocation() const
{
    return _projectLocation;
}

//-------------------------------------------------------------------
void
NewProjectDialog::onOk(wxCommandEvent& event)
{
    // get window width
    wxString windowWidthStr = _windowWidthEntry->GetValue();
    unsigned long windowWidth;
    if (windowWidthStr.IsEmpty() || !windowWidthStr.ToULong(&windowWidth) || windowWidth > MAX_WINDOW_WIDTH) {
        wxMessageDialog errorDialog(this, wxEmptyString, wxT("New Project"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        errorDialog.SetMessage(wxT("Invalid window width."));
        errorDialog.ShowModal();
        return;
    }

    // get window height
    wxString windowHeightStr = _windowHeightEntry->GetValue();
    unsigned long windowHeight;
    if (windowHeightStr.IsEmpty() || !windowHeightStr.ToULong(&windowHeight) || windowHeight > MAX_WINDOW_HEIGHT) {
        wxMessageDialog errorDialog(this, wxEmptyString, wxT("New Project"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        errorDialog.SetMessage(wxT("Invalid window height."));
        errorDialog.ShowModal();
        return;
    }

    // get window full screen (no validation need)
    bool windowFullScreen = _windowFullScreenCheckBox->GetValue();

    // get name (no validation need)
    wxString name = _nameEntry->GetValue();

    // get description (no validation need)
    wxString description = _descriptionEntry->GetValue();

    // get entry point (no validation need)
    wxString entryPoint = _entryPointEntry->GetValue();

    // get location (no validation need)
    wxString location = _locationEntry->GetValue();

    // apply properties
    _projectProperties.Name        = name;
    _projectProperties.Description = description;
    _projectProperties.EntryPoint  = entryPoint;

    _projectProperties.Window.Width      = windowWidth;
    _projectProperties.Window.Height     = windowHeight;
    _projectProperties.Window.FullScreen = windowFullScreen;

    _projectLocation = location;

    // return ok
    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
NewProjectDialog::onCancel(wxCommandEvent& event)
{
    // return cancel
    EndModal(wxID_CANCEL);
}

//-------------------------------------------------------------------
void
NewProjectDialog::onLocationPicked(wxFileDirPickerEvent& event)
{
    _locationEntry->SetValue(event.GetPath());
}
