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

#include "AddNewFileDialog.hpp"
#include "Developer.hpp"

#define ADDNEWFILEDIALOG_STYLE (wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU)


//-------------------------------------------------------------------
AddNewFileDialog::AddNewFileDialog(const wxString& defaultLocation)
    : wxDialog(0, wxID_ANY, wxT("Add New File"), wxDefaultPosition, wxSize(400, 350), ADDNEWFILEDIALOG_STYLE)
    , _selectedFileType(FT_UNKNOWN)
{
    SetMinSize(wxSize(400, 350));

    wxListCtrl* fileTypeList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_SINGLE_SEL);

    wxImageList* imageList = new wxImageList(32, 32);
    int scriptFileTypeImageID = imageList->Add(wxBitmap(wxT("system/developer/icons/file-type-script-large.png"), wxBITMAP_TYPE_PNG));
    int textFileTypeImageID   = imageList->Add(wxBitmap(wxT("system/developer/icons/file-type-text-large.png"),   wxBITMAP_TYPE_PNG));
    fileTypeList->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);

    fileTypeList->InsertItem(ID_FT_SCRIPT, wxT("Script File"), scriptFileTypeImageID);
    fileTypeList->InsertItem(ID_FT_TEXT,   wxT("Text File"),   textFileTypeImageID);

    wxStaticText* nameEntryLabel     = new wxStaticText(this, wxID_ANY, wxT("Name:"));
    wxStaticText* locationEntryLabel = new wxStaticText(this, wxID_ANY, wxT("Location:"));

    _nameEntry     = new wxTextCtrl(this, wxID_ANY, wxT("<enter name>"));
    _locationEntry = new wxTextCtrl(this, wxID_ANY, defaultLocation);

    wxDirPickerCtrl* locationPicker = new wxDirPickerCtrl(this, wxID_ANY, defaultLocation, wxT("File Location"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST);

    wxFlexGridSizer* entries = new wxFlexGridSizer(2, 3, 5, 5);

    entries->Add(nameEntryLabel);
    entries->Add(_nameEntry, 1, wxEXPAND);
    entries->Add(new wxStaticText(this, wxID_ANY, wxT("")));

    entries->Add(locationEntryLabel);
    entries->Add(_locationEntry, 1, wxEXPAND);
    entries->Add(locationPicker);

    entries->AddGrowableCol(1, 1);

    wxButton* okButton     = new wxButton(this, wxID_OK,     wxT("OK"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);

    buttons->Add(okButton);
    buttons->AddSpacer(5);
    buttons->Add(cancelButton);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(fileTypeList, 1, wxEXPAND      | wxALL, 10);
    sizer->Add(entries,      0, wxEXPAND      | wxALL, 10);
    sizer->Add(buttons,      0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(sizer);

    // set icon
    SetIcon(wxIcon(wxT("system/developer/icons/file-add-new.png"), wxBITMAP_TYPE_PNG));

    // connect command events
    Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,   wxListEventHandler(AddNewFileDialog::onFileTypeSelected));
    Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(AddNewFileDialog::onFileTypeDeselected));
    Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,  wxListEventHandler(AddNewFileDialog::onFileTypeActivated));

    Connect(wxID_OK,     wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddNewFileDialog::onOk));
    Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddNewFileDialog::onCancel));

    Connect(wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(AddNewFileDialog::onLocationPicked));

    Connect(wxID_OK, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AddNewFileDialog::onUpdateUI_Ok));
}

//-------------------------------------------------------------------
AddNewFileDialog::~AddNewFileDialog()
{
}

//-------------------------------------------------------------------
int
AddNewFileDialog::getSelectedFileType() const
{
    return _selectedFileType;
}

//-------------------------------------------------------------------
const wxString&
AddNewFileDialog::getFileName() const
{
    return _fileName;
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onFileTypeSelected(wxListEvent& event)
{
    switch (event.GetIndex()) {
        case ID_FT_SCRIPT:
            _selectedFileType = FT_SCRIPT;
            break;
        case ID_FT_TEXT:
            _selectedFileType = FT_TEXT;
            break;
        default:
            _selectedFileType = FT_UNKNOWN;
    }
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onFileTypeDeselected(wxListEvent& event)
{
    _selectedFileType = FT_UNKNOWN;
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onFileTypeActivated(wxListEvent& event)
{
    wxString fileName = _nameEntry->GetValue();

    if (fileName == wxT("<enter name>")) {
        wxMessageDialog errorDialog(this, wxEmptyString, wxT("Add New File"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        errorDialog.SetMessage(wxT("Enter a file name."));
        errorDialog.ShowModal();
        return;
    }

    wxString forbiddenChars = wxFileName::GetForbiddenChars();
    for (size_t i = 0; i < forbiddenChars.Length(); i++) {
        if (fileName.Find(forbiddenChars.GetChar(i)) != wxNOT_FOUND) {
            wxMessageDialog errorDialog(this, wxEmptyString, wxT("Add New File"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            errorDialog.SetMessage(wxT("File name cannot contain any of the following characters: ") + forbiddenChars);
            errorDialog.ShowModal();
            return;
        }
    }

    _fileName = _locationEntry->GetValue() + wxFileName::GetPathSeparator() + fileName;

    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onOk(wxCommandEvent& event)
{
    wxString fileName = _nameEntry->GetValue();

    if (fileName == wxT("<enter name>")) {
        wxMessageDialog errorDialog(this, wxEmptyString, wxT("Add New File"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        errorDialog.SetMessage(wxT("Enter a file name."));
        errorDialog.ShowModal();
        return;
    }

    wxString forbiddenChars = wxFileName::GetForbiddenChars();
    for (size_t i = 0; i < forbiddenChars.Length(); i++) {
        if (fileName.Find(forbiddenChars.GetChar(i)) != wxNOT_FOUND) {
            wxMessageDialog errorDialog(this, wxEmptyString, wxT("Add New File"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            errorDialog.SetMessage(wxT("File name cannot contain any of the following characters: ") + forbiddenChars);
            errorDialog.ShowModal();
            return;
        }
    }

    _fileName = _locationEntry->GetValue() + wxFileName::GetPathSeparator() + fileName;

    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onLocationPicked(wxFileDirPickerEvent& event)
{
    _locationEntry->SetValue(event.GetPath());
}

//-------------------------------------------------------------------
void
AddNewFileDialog::onUpdateUI_Ok(wxUpdateUIEvent& event)
{
    if (_selectedFileType != FT_UNKNOWN && !_nameEntry->IsEmpty() && !_locationEntry->IsEmpty()) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}
