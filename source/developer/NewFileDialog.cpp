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

#include "NewFileDialog.hpp"
#include "Developer.hpp"

#define NEWFILEDIALOG_STYLE (wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU)


//-------------------------------------------------------------------
NewFileDialog::NewFileDialog()
    : wxDialog(0, wxID_ANY, wxT("New File"), wxDefaultPosition, wxSize(400, 300), NEWFILEDIALOG_STYLE)
    , _selectedFileType(FT_UNKNOWN)
{
    SetMinSize(wxSize(400, 300));

    wxListCtrl* fileTypeList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON | wxLC_SINGLE_SEL);

    wxImageList* imageList = new wxImageList(32, 32);
    int scriptFileTypeImageID = imageList->Add(wxBitmap(wxT("system/developer/icons/file-type-script-large.png"), wxBITMAP_TYPE_PNG));
    int textFileTypeImageID   = imageList->Add(wxBitmap(wxT("system/developer/icons/file-type-text-large.png"),   wxBITMAP_TYPE_PNG));
    fileTypeList->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);

    fileTypeList->InsertItem(ID_FT_SCRIPT, wxT("Script File"), scriptFileTypeImageID);
    fileTypeList->InsertItem(ID_FT_TEXT,   wxT("Text File"),   textFileTypeImageID);

    wxButton* okButton     = new wxButton(this, wxID_OK,     wxT("OK"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);

    buttons->Add(okButton);
    buttons->AddSpacer(5);
    buttons->Add(cancelButton);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(fileTypeList, 1, wxEXPAND      | wxALL, 10);
    sizer->Add(buttons,      0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(sizer);

    // set icon
    SetIcon(wxIcon(wxT("system/developer/icons/file-new.png"), wxBITMAP_TYPE_PNG));

    Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,   wxListEventHandler(NewFileDialog::onFileTypeSelected));
    Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(NewFileDialog::onFileTypeDeselected));
    Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,  wxListEventHandler(NewFileDialog::onFileTypeActivated));

    Connect(wxID_OK,     wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewFileDialog::onOk));
    Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewFileDialog::onCancel));

    Connect(wxID_OK, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewFileDialog::onUpdateUI_Ok));
}

//-------------------------------------------------------------------
NewFileDialog::~NewFileDialog()
{
}

//-------------------------------------------------------------------
int
NewFileDialog::getSelectedFileType() const
{
    return _selectedFileType;
}

//-------------------------------------------------------------------
void
NewFileDialog::onFileTypeSelected(wxListEvent& event)
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
NewFileDialog::onFileTypeDeselected(wxListEvent& event)
{
    _selectedFileType = FT_UNKNOWN;
}

//-------------------------------------------------------------------
void
NewFileDialog::onFileTypeActivated(wxListEvent& event)
{
    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
NewFileDialog::onOk(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
NewFileDialog::onCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

//-------------------------------------------------------------------
void
NewFileDialog::onUpdateUI_Ok(wxUpdateUIEvent& event)
{
    if (_selectedFileType != FT_UNKNOWN) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
}
