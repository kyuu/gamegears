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

#include "SaveChangesDialog.hpp"

#define SAVECHANGESDIALOG_STYLE (wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU)

#define MAX_WINDOW_WIDTH  4096
#define MAX_WINDOW_HEIGHT 4096


//-------------------------------------------------------------------
SaveChangesDialog::SaveChangesDialog(const wxArrayString& itemList)
    : wxDialog(0, wxID_ANY, wxT("Developer"), wxDefaultPosition, wxSize(400, 300), SAVECHANGESDIALOG_STYLE)
{
    SetMinSize(wxSize(400, 300));

    // create box 1
    wxBoxSizer* box1 = new wxBoxSizer(wxVERTICAL);

    wxStaticText* questionStaticText = new wxStaticText(this, wxID_ANY, wxT("Save changes to the following files?"));

    wxTextCtrl* itemListTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_DONTWRAP);

    for (size_t i = 0; i < itemList.GetCount(); i++) {
        *itemListTextCtrl << itemList.Item(i) + wxT("\n");
    }

    box1->Add(questionStaticText, 0,            wxALL, 5);
    box1->Add(itemListTextCtrl,   1, wxEXPAND | wxALL, 5);

    wxButton* yesButton    = new wxButton(this, wxID_YES,    wxT("Yes"));
    wxButton* noButton     = new wxButton(this, wxID_NO,     wxT("No"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    // create box 2
    wxBoxSizer* box2 = new wxBoxSizer(wxHORIZONTAL);

    box2->Add(yesButton);
    box2->AddSpacer(5);
    box2->Add(noButton);
    box2->AddSpacer(5);
    box2->Add(cancelButton);

    // create main sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(box1, 1, wxEXPAND      | wxALL, 10);
    sizer->Add(box2, 0, wxALIGN_RIGHT | wxALL, 10);

    // apply sizer
    SetSizer(sizer);

    // set initial focus to the cancel button
    yesButton->SetFocus();

    // connect events
    Connect(wxID_YES,    wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveChangesDialog::onYes));
    Connect(wxID_NO,     wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveChangesDialog::onNo));
    Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveChangesDialog::onCancel));
}

//-------------------------------------------------------------------
SaveChangesDialog::~SaveChangesDialog()
{
}

//-------------------------------------------------------------------
void
SaveChangesDialog::onYes(wxCommandEvent& event)
{
    // return yes
    EndModal(wxID_YES);
}

//-------------------------------------------------------------------
void
SaveChangesDialog::onNo(wxCommandEvent& event)
{
    // return no
    EndModal(wxID_NO);
}

//-------------------------------------------------------------------
void
SaveChangesDialog::onCancel(wxCommandEvent& event)
{
    // return cancel
    EndModal(wxID_CANCEL);
}
