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

#include <wx/filename.h>
#include "NewDirectoryDialog.hpp"

#define NEWDIRECTORYDIALOG_STYLE (wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU)


//-------------------------------------------------------------------
NewDirectoryDialog::NewDirectoryDialog()
    : wxDialog(0, wxID_ANY, wxT("New Directory"), wxDefaultPosition, wxSize(400, 130), NEWDIRECTORYDIALOG_STYLE)
{
    SetMinSize(wxSize(400, 130));

    wxStaticText* nameLabel = new wxStaticText(this, wxID_ANY, wxT("Name:"));

    _nameEntry = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    wxBoxSizer* box1 = new wxBoxSizer(wxHORIZONTAL);

    box1->Add(nameLabel,  0,            wxALL, 5);
    box1->Add(_nameEntry, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* box2 = new wxBoxSizer(wxHORIZONTAL);

    wxButton* okButton     = new wxButton(this, wxID_OK,     wxT("OK"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    box2->Add(okButton);
    box2->AddSpacer(5);
    box2->Add(cancelButton);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(box1, 0, wxEXPAND      | wxALL, 10);
    sizer->Add(box2, 0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(sizer);

    _nameEntry->SetFocus();

    // set icon
    SetIcon(wxIcon(wxT("system/developer/icons/directory-new.png"), wxBITMAP_TYPE_PNG));

    // connect events
    Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NewDirectoryDialog::onEnter));

    Connect(wxID_OK,     wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDirectoryDialog::onOk));
    Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDirectoryDialog::onCancel));

    Connect(wxID_OK, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewDirectoryDialog::onUpdateUI_Ok));
}

//-------------------------------------------------------------------
NewDirectoryDialog::~NewDirectoryDialog()
{
}

//-------------------------------------------------------------------
const wxString&
NewDirectoryDialog::getDirectoryName() const
{
    return _dirName;
}

//-------------------------------------------------------------------
void
NewDirectoryDialog::onEnter(wxCommandEvent& event)
{
    onOk(event);
}

//-------------------------------------------------------------------
void
NewDirectoryDialog::onOk(wxCommandEvent& event)
{
    wxString dirName = _nameEntry->GetValue();

    wxString forbiddenChars = wxFileName::GetForbiddenChars();
    for (size_t i = 0; i < forbiddenChars.Length(); i++) {
        if (dirName.Find(forbiddenChars.GetChar(i)) != wxNOT_FOUND) {
            wxMessageDialog errorDialog(this, wxEmptyString, wxT("New Directory"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            errorDialog.SetMessage(wxT("Directory name cannot contain any of the following characters: ") + forbiddenChars);
            errorDialog.ShowModal();
            return;
        }
    }

    _dirName = dirName;

    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
NewDirectoryDialog::onCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

//-------------------------------------------------------------------
void
NewDirectoryDialog::onUpdateUI_Ok(wxUpdateUIEvent& event)
{
    if (!_nameEntry->IsEmpty()) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}
