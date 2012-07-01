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

#include <wx/treebook.h>
#include <wx/fontenum.h>
#include "PreferencesDialog.hpp"

#define PREFERENCESDIALOG_STYLE (wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU)


//-------------------------------------------------------------------
PreferencesDialog::PreferencesDialog(const Preferences& initialPreferences, const wxString& initialPath)
    : wxDialog(0, wxID_ANY, wxT("Preferences"), wxDefaultPosition, wxSize(550, 450), PREFERENCESDIALOG_STYLE)
    , _preferences(initialPreferences)
    , _initialPath(initialPath)
{
    SetMinSize(wxSize(550, 450));

    wxTreebook* pageContainer = new wxTreebook(this, wxID_ANY);

    pageContainer->AddPage(0, wxT("Developer")); // empty page
    pageContainer->AddSubPage(createDeveloperGeneralPage(pageContainer), wxT("General"));

    pageContainer->AddPage(0, wxT("Text Editor")); // empty page
    pageContainer->AddSubPage(createTextEditorGeneralPage(pageContainer), wxT("General"));
    pageContainer->AddSubPage(createTextEditorMarginPage(pageContainer),  wxT("Margin"));
    pageContainer->AddSubPage(createTextEditorIndentPage(pageContainer),  wxT("Indent"));
    pageContainer->AddSubPage(createTextEditorBracesPage(pageContainer),  wxT("Braces"));
    pageContainer->AddSubPage(createTextEditorSyntaxPage(pageContainer),  wxT("Syntax"));

    wxButton* loadDefaultsButton = new wxButton(this, ID_RESTORE_DEFAULTS, wxT("Restore Defaults"));

    wxButton* okButton     = new wxButton(this, wxID_OK,     wxT("OK"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    wxBoxSizer* buttonBox = new wxBoxSizer(wxHORIZONTAL);

    buttonBox->Add(loadDefaultsButton);
    buttonBox->Add(new wxStaticText(this, wxID_ANY, wxEmptyString), 1, wxEXPAND);
    buttonBox->Add(okButton);
    buttonBox->AddSpacer(5);
    buttonBox->Add(cancelButton);

    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);

    dialogSizer->Add(pageContainer, 1, wxEXPAND | wxALL, 10);
    dialogSizer->Add(buttonBox,     0, wxEXPAND | wxALL, 10);

    SetSizer(dialogSizer);

    // initialize controls
    loadValues();

    // set icon
    SetIcon(wxIcon(wxT("system/developer/icons/preferences.png"), wxBITMAP_TYPE_PNG));

    // connect events
    Connect(ID_RESTORE_DEFAULTS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::onRestoreDefaults));
    Connect(wxID_OK,             wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::onOk));
    Connect(wxID_CANCEL,         wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::onCancel));
}

//-------------------------------------------------------------------
PreferencesDialog::~PreferencesDialog()
{
}

//-------------------------------------------------------------------
const Preferences&
PreferencesDialog::getPreferences() const
{
    return _preferences;
}

//-------------------------------------------------------------------
void
PreferencesDialog::loadValues()
{
    // developer general
    _dataLocationCtrl->SetValue(_preferences.General.DataLocation);
    _commonDataLocationCtrl->SetValue(_preferences.General.CommonDataLocation);

    _openLastProjectCtrl->SetValue(_preferences.General.OpenLastProjectOnStartup);

    _projectFileExtensionList->SetStrings(_preferences.General.FileTypeAssociation.Project);
    _scriptFileExtensionList->SetStrings(_preferences.General.FileTypeAssociation.Script);
    _textFileExtensionList->SetStrings(_preferences.General.FileTypeAssociation.Text);

    // text editor general
    if (!_preferences.TextEditor.General.Font.FaceName.IsEmpty()) {
        wxFont font;
        font.SetFaceName(_preferences.TextEditor.General.Font.FaceName);
        font.SetPointSize(_preferences.TextEditor.General.Font.PointSize);
        _fontCtrl->SetSelectedFont(font);
    }

    _initialZoomLevelCtrl->SetValue(_preferences.TextEditor.General.InitialZoomLevel);

    //_enableAutoCompleteCtrl->SetValue(_preferences.TextEditor.General.EnableAutoComplete);
    //_enableCallTipsCtrl->SetValue(_preferences.TextEditor.General.EnableCallTips);

    // text editor margin
    _displayLineNumbersCtrl->SetValue(_preferences.TextEditor.Margin.DisplayLineNumbers);

    _lineNumberStyleBoldCtrl->SetValue(_preferences.TextEditor.Margin.LineNumberStyle.Bold);
    _lineNumberStyleItalicCtrl->SetValue(_preferences.TextEditor.Margin.LineNumberStyle.Italic);

    _lineNumberStyleForegroundCtrl->SetColour(_preferences.TextEditor.Margin.LineNumberStyle.Foreground);
    _lineNumberStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Margin.LineNumberStyle.Background);

    _displayFoldersCtrl->SetValue(_preferences.TextEditor.Margin.DisplayFolders);
    _foldAllOnStartupCtrl->SetValue(_preferences.TextEditor.Margin.FoldAllOnStartup);

    _folderHeaderStyleShapeCtrl->SetSelection(_preferences.TextEditor.Margin.FolderHeaderStyle.Shape);
    _folderHeaderStyleForegroundCtrl->SetColour(_preferences.TextEditor.Margin.FolderHeaderStyle.Foreground);
    _folderHeaderStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Margin.FolderHeaderStyle.Background);

    _folderMidHeaderStyleShapeCtrl->SetSelection(_preferences.TextEditor.Margin.FolderMidHeaderStyle.Shape);
    _folderMidHeaderStyleForegroundCtrl->SetColour(_preferences.TextEditor.Margin.FolderMidHeaderStyle.Foreground);
    _folderMidHeaderStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Margin.FolderMidHeaderStyle.Background);

    _folderBodyStyleShapeCtrl->SetSelection(_preferences.TextEditor.Margin.FolderBodyStyle.Shape);
    _folderBodyStyleForegroundCtrl->SetColour(_preferences.TextEditor.Margin.FolderBodyStyle.Foreground);
    _folderBodyStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Margin.FolderBodyStyle.Background);

    // text editor indent
    wxString tabSizeStr;
    tabSizeStr << _preferences.TextEditor.Indent.TabSize;
    _tabSizeCtrl->SetValue(tabSizeStr);

    _convertTabsToSpacesCtrl->SetValue(_preferences.TextEditor.Indent.ConvertTabsToSpaces);

    wxString indentSizeStr;
    indentSizeStr << _preferences.TextEditor.Indent.IndentSize;
    _indentSizeCtrl->SetValue(indentSizeStr);

    _enableAutoIndentCtrl->SetValue(_preferences.TextEditor.Indent.EnableAutoIndent);
    _backspaceUnindentsCtrl->SetValue(_preferences.TextEditor.Indent.BackspaceUnindents);
    _showIndentationGuidesCtrl->SetValue(_preferences.TextEditor.Indent.ShowIndentationGuides);

    // text editor braces
    _enableBraceMatchingCtrl->SetValue(_preferences.TextEditor.Braces.EnableBraceMatching);

    _braceHighlightStyleBoldCtrl->SetValue(_preferences.TextEditor.Braces.HighlightStyle.Bold);
    _braceHighlightStyleItalicCtrl->SetValue(_preferences.TextEditor.Braces.HighlightStyle.Italic);

    _braceHighlightStyleForegroundCtrl->SetColour(_preferences.TextEditor.Braces.HighlightStyle.Foreground);
    _braceHighlightStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Braces.HighlightStyle.Background);

    _braceBadlightStyleBoldCtrl->SetValue(_preferences.TextEditor.Braces.BadlightStyle.Bold);
    _braceBadlightStyleItalicCtrl->SetValue(_preferences.TextEditor.Braces.BadlightStyle.Italic);

    _braceBadlightStyleForegroundCtrl->SetColour(_preferences.TextEditor.Braces.BadlightStyle.Foreground);
    _braceBadlightStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Braces.BadlightStyle.Background);

    // text editor syntax
    _enableSyntaxHighlightingCtrl->SetValue(_preferences.TextEditor.Syntax.EnableSyntaxHighlighting);

    _operatorStyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Operator.Style.Bold);
    _operatorStyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Operator.Style.Italic);

    _operatorStyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Operator.Style.Foreground);
    _operatorStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Operator.Style.Background);

    _stringStyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.String.Style.Bold);
    _stringStyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.String.Style.Italic);

    _stringStyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.String.Style.Foreground);
    _stringStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.String.Style.Background);

    _identifierStyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Identifier.Style.Bold);
    _identifierStyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Identifier.Style.Italic);

    _identifierStyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Identifier.Style.Foreground);
    _identifierStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Identifier.Style.Background);

    _numberStyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Number.Style.Bold);
    _numberStyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Number.Style.Italic);

    _numberStyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Number.Style.Foreground);
    _numberStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Number.Style.Background);

    _characterStyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Character.Style.Bold);
    _characterStyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Character.Style.Italic);

    _characterStyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Character.Style.Foreground);
    _characterStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Character.Style.Background);

    _commentStyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Comment.Style.Bold);
    _commentStyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Comment.Style.Italic);

    _commentStyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Comment.Style.Foreground);
    _commentStyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Comment.Style.Background);

    _keyword1StyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Keyword1.Style.Bold);
    _keyword1StyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Keyword1.Style.Italic);

    _keyword1StyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Keyword1.Style.Foreground);
    _keyword1StyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Keyword1.Style.Background);

    _keyword1List->SetStrings(_preferences.TextEditor.Syntax.Keyword1.List);

    _keyword2StyleBoldCtrl->SetValue(_preferences.TextEditor.Syntax.Keyword2.Style.Bold);
    _keyword2StyleItalicCtrl->SetValue(_preferences.TextEditor.Syntax.Keyword2.Style.Italic);

    _keyword2StyleForegroundCtrl->SetColour(_preferences.TextEditor.Syntax.Keyword2.Style.Foreground);
    _keyword2StyleBackgroundCtrl->SetColour(_preferences.TextEditor.Syntax.Keyword2.Style.Background);

    _keyword2List->SetStrings(_preferences.TextEditor.Syntax.Keyword2.List);
}

//-------------------------------------------------------------------
void
PreferencesDialog::saveValues()
{
    // developer general
    _preferences.General.DataLocation       = _dataLocationCtrl->GetValue();
    _preferences.General.CommonDataLocation = _commonDataLocationCtrl->GetValue();

    _preferences.General.OpenLastProjectOnStartup = _openLastProjectCtrl->GetValue();

    _projectFileExtensionList->GetStrings(_preferences.General.FileTypeAssociation.Project);
    _scriptFileExtensionList->GetStrings(_preferences.General.FileTypeAssociation.Script);
    _textFileExtensionList->GetStrings(_preferences.General.FileTypeAssociation.Text);

    // text editor general
    wxFont selectedFont = _fontCtrl->GetSelectedFont();
    _preferences.TextEditor.General.Font.PointSize = selectedFont.GetPointSize();
    _preferences.TextEditor.General.Font.FaceName  = selectedFont.GetFaceName();

    _preferences.TextEditor.General.InitialZoomLevel = _initialZoomLevelCtrl->GetValue();

    //_preferences.TextEditor.General.EnableAutoComplete = _enableAutoCompleteCtrl->GetValue();
    //_preferences.TextEditor.General.EnableCallTips     = _enableCallTipsCtrl->GetValue();

    // text editor margin
    _preferences.TextEditor.Margin.DisplayLineNumbers = _displayLineNumbersCtrl->GetValue();

    _preferences.TextEditor.Margin.LineNumberStyle.Bold   = _lineNumberStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Margin.LineNumberStyle.Italic = _lineNumberStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Margin.LineNumberStyle.Foreground = _lineNumberStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Margin.LineNumberStyle.Background = _lineNumberStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Margin.DisplayFolders   = _displayFoldersCtrl->GetValue();
    _preferences.TextEditor.Margin.FoldAllOnStartup = _foldAllOnStartupCtrl->GetValue();

    _preferences.TextEditor.Margin.FolderHeaderStyle.Shape      = _folderHeaderStyleShapeCtrl->GetSelection();
    _preferences.TextEditor.Margin.FolderHeaderStyle.Foreground = _folderHeaderStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Margin.FolderHeaderStyle.Background = _folderHeaderStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Margin.FolderMidHeaderStyle.Shape      = _folderMidHeaderStyleShapeCtrl->GetSelection();
    _preferences.TextEditor.Margin.FolderMidHeaderStyle.Foreground = _folderMidHeaderStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Margin.FolderMidHeaderStyle.Background = _folderMidHeaderStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Margin.FolderBodyStyle.Shape      = _folderBodyStyleShapeCtrl->GetSelection();
    _preferences.TextEditor.Margin.FolderBodyStyle.Foreground = _folderBodyStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Margin.FolderBodyStyle.Background = _folderBodyStyleBackgroundCtrl->GetColour();

    // text editor indent
    _preferences.TextEditor.Indent.TabSize = _tabSizeCtrl->GetValue();

    _preferences.TextEditor.Indent.ConvertTabsToSpaces = _convertTabsToSpacesCtrl->GetValue();

    _preferences.TextEditor.Indent.IndentSize = _indentSizeCtrl->GetValue();

    _preferences.TextEditor.Indent.EnableAutoIndent      = _enableAutoIndentCtrl->GetValue();
    _preferences.TextEditor.Indent.BackspaceUnindents    = _backspaceUnindentsCtrl->GetValue();
    _preferences.TextEditor.Indent.ShowIndentationGuides = _showIndentationGuidesCtrl->GetValue();

    // text editor braces
    _preferences.TextEditor.Braces.EnableBraceMatching   = _enableBraceMatchingCtrl->GetValue();

    _preferences.TextEditor.Braces.HighlightStyle.Bold   = _braceHighlightStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Braces.HighlightStyle.Italic = _braceHighlightStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Braces.HighlightStyle.Foreground = _braceHighlightStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Braces.HighlightStyle.Background = _braceHighlightStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Braces.BadlightStyle.Bold   = _braceBadlightStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Braces.BadlightStyle.Italic = _braceBadlightStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Braces.BadlightStyle.Foreground = _braceBadlightStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Braces.BadlightStyle.Background = _braceBadlightStyleBackgroundCtrl->GetColour();

    // text editor syntax
    _preferences.TextEditor.Syntax.EnableSyntaxHighlighting = _enableSyntaxHighlightingCtrl->GetValue();

    _preferences.TextEditor.Syntax.Operator.Style.Bold   = _operatorStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Operator.Style.Italic = _operatorStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Operator.Style.Foreground = _operatorStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Operator.Style.Background = _operatorStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Syntax.String.Style.Bold   = _stringStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.String.Style.Italic = _stringStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.String.Style.Foreground = _stringStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.String.Style.Background = _stringStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Syntax.Identifier.Style.Bold   = _identifierStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Identifier.Style.Italic = _identifierStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Identifier.Style.Foreground = _identifierStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Identifier.Style.Background = _identifierStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Syntax.Number.Style.Bold   = _numberStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Number.Style.Italic = _numberStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Number.Style.Foreground = _numberStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Number.Style.Background = _numberStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Syntax.Character.Style.Bold   = _characterStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Character.Style.Italic = _characterStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Character.Style.Foreground = _characterStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Character.Style.Background = _characterStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Syntax.Comment.Style.Bold   = _commentStyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Comment.Style.Italic = _commentStyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Comment.Style.Foreground = _commentStyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Comment.Style.Background = _commentStyleBackgroundCtrl->GetColour();

    _preferences.TextEditor.Syntax.Keyword1.Style.Bold   = _keyword1StyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Keyword1.Style.Italic = _keyword1StyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Keyword1.Style.Foreground = _keyword1StyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Keyword1.Style.Background = _keyword1StyleBackgroundCtrl->GetColour();

    _keyword1List->GetStrings(_preferences.TextEditor.Syntax.Keyword1.List);

    _preferences.TextEditor.Syntax.Keyword2.Style.Bold   = _keyword2StyleBoldCtrl->GetValue();
    _preferences.TextEditor.Syntax.Keyword2.Style.Italic = _keyword2StyleItalicCtrl->GetValue();

    _preferences.TextEditor.Syntax.Keyword2.Style.Foreground = _keyword2StyleForegroundCtrl->GetColour();
    _preferences.TextEditor.Syntax.Keyword2.Style.Background = _keyword2StyleBackgroundCtrl->GetColour();

    _keyword2List->GetStrings(_preferences.TextEditor.Syntax.Keyword2.List);

}

//-------------------------------------------------------------------
wxWindow*
PreferencesDialog::createDeveloperGeneralPage(wxWindow* parentCtrl)
{
    wxPanel* panel = new wxPanel(parentCtrl, wxID_ANY);

    wxStaticBoxSizer* box0 = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("File type association"));

    wxListbook* fileTypeList = new wxListbook(box0->GetStaticBox(), wxID_ANY);

    _projectFileExtensionList = new wxEditableListBox(fileTypeList, wxID_ANY, wxT("Associated File Extensions"), wxDefaultPosition, wxDefaultSize, wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);
    _scriptFileExtensionList  = new wxEditableListBox(fileTypeList, wxID_ANY, wxT("Associated File Extensions"), wxDefaultPosition, wxDefaultSize, wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);
    _textFileExtensionList    = new wxEditableListBox(fileTypeList, wxID_ANY, wxT("Associated File Extensions"), wxDefaultPosition, wxDefaultSize, wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);

    fileTypeList->AddPage(_projectFileExtensionList, wxT("Project File"));
    fileTypeList->AddPage(_scriptFileExtensionList,  wxT("Script File"));
    fileTypeList->AddPage(_textFileExtensionList,    wxT("Text File"));

    box0->Add(fileTypeList, 1, wxEXPAND | wxALL, 5);

    wxStaticText* dataLocationLabel       = new wxStaticText(panel, wxID_ANY, wxT("Data:"));
    wxStaticText* commonDataLocationLabel = new wxStaticText(panel, wxID_ANY, wxT("Common Data:"));

    _dataLocationCtrl       = new wxTextCtrl(panel, wxID_ANY);
    _commonDataLocationCtrl = new wxTextCtrl(panel, wxID_ANY);

    wxDirPickerCtrl* browseDataLocationCtrl       = new wxDirPickerCtrl(panel, ID_BROWSE_DATA_LOCATION,        _initialPath, wxT("Data Location"),        wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST);
    wxDirPickerCtrl* browseCommonDataLocationCtrl = new wxDirPickerCtrl(panel, ID_BROWSE_COMMON_DATA_LOCATION, _initialPath, wxT("Common Data Location"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST);

    wxStaticBoxSizer* box1 = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Data location"));

    wxFlexGridSizer* fgs1 = new wxFlexGridSizer(2, 3, 5, 5);

    fgs1->Add(dataLocationLabel);
    fgs1->Add(_dataLocationCtrl, 1, wxEXPAND);
    fgs1->Add(browseDataLocationCtrl);

    fgs1->Add(commonDataLocationLabel);
    fgs1->Add(_commonDataLocationCtrl, 1, wxEXPAND);
    fgs1->Add(browseCommonDataLocationCtrl);

    fgs1->AddGrowableCol(1, 1);

    box1->Add(fgs1, 1, wxEXPAND | wxALL, 5);

    _openLastProjectCtrl = new wxCheckBox(panel, wxID_ANY, wxT("Open last project on startup"));

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    panelSizer->Add(box0,                 1, wxEXPAND | wxALL, 5);
    panelSizer->Add(box1,                 0, wxEXPAND | wxALL, 5);
    panelSizer->Add(_openLastProjectCtrl, 0,            wxALL, 5);

    panel->SetSizer(panelSizer);

    // connect events
    Connect(ID_BROWSE_DATA_LOCATION,        wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(PreferencesDialog::onDataLocationPicked));
    Connect(ID_BROWSE_COMMON_DATA_LOCATION, wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(PreferencesDialog::onCommonDataLocationPicked));

    return panel;
}

//-------------------------------------------------------------------
wxWindow*
PreferencesDialog::createTextEditorGeneralPage(wxWindow* parentCtrl)
{
    wxPanel* panel = new wxPanel(parentCtrl, wxID_ANY);

    // create widgets
    wxStaticBoxSizer* sbs1 = new wxStaticBoxSizer(wxHORIZONTAL, panel, wxT("Font"));
    wxStaticBoxSizer* sbs2 = new wxStaticBoxSizer(wxHORIZONTAL, panel, wxT("Zooming"));

    wxStaticText* initialZoomLevelLabel = new wxStaticText(sbs2->GetStaticBox(), wxID_ANY, wxT("Initial zoom level:"));

    _fontCtrl = new wxFontPickerCtrl(sbs1->GetStaticBox(), wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_FONTDESC_AS_LABEL);

    _initialZoomLevelCtrl = new wxSpinCtrl(sbs2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_CENTRE, -10, 20);

    //_enableAutoCompleteCtrl  = new wxCheckBox(panel, wxID_ANY, wxT("Enable auto-complete"));
    //_enableCallTipsCtrl      = new wxCheckBox(panel, wxID_ANY, wxT("Enable call tips"));

    // create sizers
    sbs1->Add(_fontCtrl, 1, wxEXPAND | wxALL, 5);

    sbs2->Add(initialZoomLevelLabel, 0, wxALL, 5);
    sbs2->Add(_initialZoomLevelCtrl, 0, wxALL, 5);

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    panelSizer->Add(sbs1,                     0, wxEXPAND | wxALL, 5);
    panelSizer->AddSpacer(10);
    panelSizer->Add(sbs2,                     0, wxEXPAND | wxALL, 5);
    //panelSizer->AddSpacer(10);
    //panelSizer->Add(_enableAutoCompleteCtrl,  0, wxEXPAND | wxALL, 5);
    //panelSizer->Add(_enableCallTipsCtrl,      0, wxEXPAND | wxALL, 5);

    panel->SetSizer(panelSizer);

    return panel;
}

//-------------------------------------------------------------------
wxWindow*
PreferencesDialog::createTextEditorMarginPage(wxWindow* parentCtrl)
{
    static int folderHeaderChoicesCount = 4;
    static wxString folderHeaderChoices[] = {
        "Arrow",
        "Plus/Minus",
        "Plus/Minus Box",
        "Plus/Minus Circle",
    };

    static int folderBodyChoicesCount = 3;
    static wxString folderBodyChoices[] = {
        "Empty",
        "Line",
        "Curved Line",
    };

    wxPanel* panel = new wxPanel(parentCtrl, wxID_ANY);

    // line numbers margin widgets
    _displayLineNumbersCtrl = new wxCheckBox(panel, wxID_ANY, wxT("Display line numbers"));

    wxStaticBoxSizer* lineNumberStyleBox = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Line number style"));

    _lineNumberStyleBoldCtrl   = new wxCheckBox(lineNumberStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _lineNumberStyleItalicCtrl = new wxCheckBox(lineNumberStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* lineNumberStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    lineNumberStyleBoldItalicBox->Add(_lineNumberStyleBoldCtrl);
    lineNumberStyleBoldItalicBox->AddSpacer(10);
    lineNumberStyleBoldItalicBox->Add(_lineNumberStyleItalicCtrl);

    _lineNumberStyleForegroundCtrl = new wxColourPickerCtrl(lineNumberStyleBox->GetStaticBox(), wxID_ANY);
    _lineNumberStyleBackgroundCtrl = new wxColourPickerCtrl(lineNumberStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* lineNumberStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    lineNumberStyleColorBox->Add(new wxStaticText(lineNumberStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    lineNumberStyleColorBox->Add(_lineNumberStyleForegroundCtrl);

    lineNumberStyleColorBox->Add(new wxStaticText(lineNumberStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    lineNumberStyleColorBox->Add(_lineNumberStyleBackgroundCtrl);

    lineNumberStyleBox->Add(lineNumberStyleBoldItalicBox, 0, wxALL, 5);
    lineNumberStyleBox->Add(lineNumberStyleColorBox,      0, wxALL, 5);

    // folders margin widgets
    _displayFoldersCtrl   = new wxCheckBox(panel, wxID_ANY, wxT("Display folders"));
    _foldAllOnStartupCtrl = new wxCheckBox(panel, wxID_ANY, wxT("Fold all on startup"));

    wxStaticBoxSizer* folderStyleBox = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Folder style"));

    wxListbook* folderStylePanels = new wxListbook(folderStyleBox->GetStaticBox(), wxID_ANY);

    folderStyleBox->Add(folderStylePanels, 1, wxEXPAND | wxALL, 5);

    wxPanel* folderHeaderStylePanel = new wxPanel(folderStylePanels, wxID_ANY);

    _folderHeaderStyleShapeCtrl = new wxChoice(folderHeaderStylePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, folderHeaderChoicesCount, folderHeaderChoices);

    _folderHeaderStyleForegroundCtrl = new wxColourPickerCtrl(folderHeaderStylePanel, wxID_ANY);
    _folderHeaderStyleBackgroundCtrl = new wxColourPickerCtrl(folderHeaderStylePanel, wxID_ANY);

    wxFlexGridSizer* folderHeaderStylePanelSizer = new wxFlexGridSizer(3, 2, 5, 5);

    folderHeaderStylePanelSizer->Add(new wxStaticText(folderHeaderStylePanel, wxID_ANY, wxT("Shape:")));
    folderHeaderStylePanelSizer->Add(_folderHeaderStyleShapeCtrl);

    folderHeaderStylePanelSizer->Add(new wxStaticText(folderHeaderStylePanel, wxID_ANY, wxT("Foreground:")));
    folderHeaderStylePanelSizer->Add(_folderHeaderStyleForegroundCtrl, 0, wxEXPAND);

    folderHeaderStylePanelSizer->Add(new wxStaticText(folderHeaderStylePanel, wxID_ANY, wxT("Background:")));
    folderHeaderStylePanelSizer->Add(_folderHeaderStyleBackgroundCtrl, 0, wxEXPAND);

    folderHeaderStylePanel->SetSizer(folderHeaderStylePanelSizer);

    wxPanel* folderMidHeaderStylePanel = new wxPanel(folderStylePanels, wxID_ANY);

    _folderMidHeaderStyleShapeCtrl = new wxChoice(folderMidHeaderStylePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, folderHeaderChoicesCount, folderHeaderChoices);

    _folderMidHeaderStyleForegroundCtrl = new wxColourPickerCtrl(folderMidHeaderStylePanel, wxID_ANY);
    _folderMidHeaderStyleBackgroundCtrl = new wxColourPickerCtrl(folderMidHeaderStylePanel, wxID_ANY);

    wxFlexGridSizer* folderMidHeaderStylePanelSizer = new wxFlexGridSizer(3, 2, 5, 5);

    folderMidHeaderStylePanelSizer->Add(new wxStaticText(folderMidHeaderStylePanel, wxID_ANY, wxT("Shape:")));
    folderMidHeaderStylePanelSizer->Add(_folderMidHeaderStyleShapeCtrl);

    folderMidHeaderStylePanelSizer->Add(new wxStaticText(folderMidHeaderStylePanel, wxID_ANY, wxT("Foreground:")));
    folderMidHeaderStylePanelSizer->Add(_folderMidHeaderStyleForegroundCtrl, 0, wxEXPAND);

    folderMidHeaderStylePanelSizer->Add(new wxStaticText(folderMidHeaderStylePanel, wxID_ANY, wxT("Background:")));
    folderMidHeaderStylePanelSizer->Add(_folderMidHeaderStyleBackgroundCtrl, 0, wxEXPAND);

    folderMidHeaderStylePanel->SetSizer(folderMidHeaderStylePanelSizer);

    wxPanel* folderBodyStylePanel = new wxPanel(folderStylePanels, wxID_ANY);

    _folderBodyStyleShapeCtrl = new wxChoice(folderBodyStylePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, folderBodyChoicesCount, folderBodyChoices);

    _folderBodyStyleForegroundCtrl = new wxColourPickerCtrl(folderBodyStylePanel, wxID_ANY);
    _folderBodyStyleBackgroundCtrl = new wxColourPickerCtrl(folderBodyStylePanel, wxID_ANY);

    wxFlexGridSizer* folderBodyStylePanelSizer = new wxFlexGridSizer(3, 2, 5, 5);

    folderBodyStylePanelSizer->Add(new wxStaticText(folderBodyStylePanel, wxID_ANY, wxT("Shape:")));
    folderBodyStylePanelSizer->Add(_folderBodyStyleShapeCtrl);

    folderBodyStylePanelSizer->Add(new wxStaticText(folderBodyStylePanel, wxID_ANY, wxT("Foreground:")));
    folderBodyStylePanelSizer->Add(_folderBodyStyleForegroundCtrl, 0, wxEXPAND);

    folderBodyStylePanelSizer->Add(new wxStaticText(folderBodyStylePanel, wxID_ANY, wxT("Background:")));
    folderBodyStylePanelSizer->Add(_folderBodyStyleBackgroundCtrl, 0, wxEXPAND);

    folderBodyStylePanel->SetSizer(folderBodyStylePanelSizer);

    folderStylePanels->AddPage(folderHeaderStylePanel,    wxT("Header"));
    folderStylePanels->AddPage(folderMidHeaderStylePanel, wxT("Mid-header"));
    folderStylePanels->AddPage(folderBodyStylePanel,      wxT("Body"));

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    panelSizer->Add(_displayLineNumbersCtrl, 0,            wxALL, 5);
    panelSizer->Add(lineNumberStyleBox,      0, wxEXPAND | wxALL, 5);
    panelSizer->AddSpacer(10);
    panelSizer->Add(_displayFoldersCtrl,     0,            wxALL, 5);
    panelSizer->Add(_foldAllOnStartupCtrl,   0,            wxALL, 5);
    panelSizer->Add(folderStyleBox,          1, wxEXPAND | wxALL, 5);

    panel->SetSizer(panelSizer);

    return panel;
}

//-------------------------------------------------------------------
wxWindow*
PreferencesDialog::createTextEditorIndentPage(wxWindow* parentCtrl)
{
    wxPanel* panel = new wxPanel(parentCtrl, wxID_ANY);

    wxStaticBoxSizer* tabBox = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Tab"));

    _tabSizeCtrl = new wxSpinCtrl(tabBox->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_CENTRE, 1, 100);

    wxBoxSizer* tabSizeBox = new wxBoxSizer(wxHORIZONTAL);

    tabSizeBox->Add(new wxStaticText(tabBox->GetStaticBox(), wxID_ANY, wxT("Tab size:")));
    tabSizeBox->AddSpacer(10);
    tabSizeBox->Add(_tabSizeCtrl);

    _convertTabsToSpacesCtrl = new wxCheckBox(tabBox->GetStaticBox(), wxID_ANY, wxT("Convert tabs to spaces"));

    tabBox->Add(tabSizeBox,               0, wxALL, 5);
    tabBox->Add(_convertTabsToSpacesCtrl, 0, wxALL, 5);

    wxStaticBoxSizer* indentBox = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Indentation"));

    _indentSizeCtrl = new wxSpinCtrl(indentBox->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_CENTRE, 1, 100);

    wxBoxSizer* indentSizeBox = new wxBoxSizer(wxHORIZONTAL);

    indentSizeBox->Add(new wxStaticText(indentBox->GetStaticBox(), wxID_ANY, wxT("Indent size:")));
    indentSizeBox->AddSpacer(10);
    indentSizeBox->Add(_indentSizeCtrl);

    _enableAutoIndentCtrl      = new wxCheckBox(indentBox->GetStaticBox(), wxID_ANY, wxT("Enable auto-indent"));
    _backspaceUnindentsCtrl    = new wxCheckBox(indentBox->GetStaticBox(), wxID_ANY, wxT("Backspace unindents"));
    _showIndentationGuidesCtrl = new wxCheckBox(indentBox->GetStaticBox(), wxID_ANY, wxT("Show indentation guides"));

    indentBox->Add(indentSizeBox,              0, wxALL, 5);
    indentBox->Add(_enableAutoIndentCtrl,      0, wxALL, 5);
    indentBox->Add(_backspaceUnindentsCtrl,    0, wxALL, 5);
    indentBox->Add(_showIndentationGuidesCtrl, 0, wxALL, 5);

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    panelSizer->Add(tabBox,    0, wxEXPAND | wxALL, 5);
    panelSizer->AddSpacer(10);
    panelSizer->Add(indentBox, 0, wxEXPAND | wxALL, 5);

    panel->SetSizer(panelSizer);

    return panel;
}

//-------------------------------------------------------------------
wxWindow*
PreferencesDialog::createTextEditorBracesPage(wxWindow* parentCtrl)
{
    wxPanel* panel = new wxPanel(parentCtrl, wxID_ANY);

    _enableBraceMatchingCtrl = new wxCheckBox(panel, wxID_ANY, wxT("Enable brace matching"));

    wxStaticBoxSizer* braceHighlightStyleBox = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Brace highlight style"));

    _braceHighlightStyleBoldCtrl   = new wxCheckBox(braceHighlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _braceHighlightStyleItalicCtrl = new wxCheckBox(braceHighlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* braceHighlightStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    braceHighlightStyleBoldItalicBox->Add(_braceHighlightStyleBoldCtrl);
    braceHighlightStyleBoldItalicBox->AddSpacer(10);
    braceHighlightStyleBoldItalicBox->Add(_braceHighlightStyleItalicCtrl);

    _braceHighlightStyleForegroundCtrl = new wxColourPickerCtrl(braceHighlightStyleBox->GetStaticBox(), wxID_ANY);
    _braceHighlightStyleBackgroundCtrl = new wxColourPickerCtrl(braceHighlightStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* braceHighlightStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    braceHighlightStyleColorBox->Add(new wxStaticText(braceHighlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    braceHighlightStyleColorBox->Add(_braceHighlightStyleForegroundCtrl);

    braceHighlightStyleColorBox->Add(new wxStaticText(braceHighlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    braceHighlightStyleColorBox->Add(_braceHighlightStyleBackgroundCtrl);

    braceHighlightStyleBox->Add(braceHighlightStyleBoldItalicBox, 0, wxALL, 5);
    braceHighlightStyleBox->Add(braceHighlightStyleColorBox,      0, wxALL, 5);

    wxStaticBoxSizer* braceBadlightStyleBox = new wxStaticBoxSizer(wxVERTICAL, panel, wxT("Brace badlight style"));

    _braceBadlightStyleBoldCtrl   = new wxCheckBox(braceBadlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _braceBadlightStyleItalicCtrl = new wxCheckBox(braceBadlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* braceBadlightStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    braceBadlightStyleBoldItalicBox->Add(_braceBadlightStyleBoldCtrl);
    braceBadlightStyleBoldItalicBox->AddSpacer(10);
    braceBadlightStyleBoldItalicBox->Add(_braceBadlightStyleItalicCtrl);

    _braceBadlightStyleForegroundCtrl = new wxColourPickerCtrl(braceBadlightStyleBox->GetStaticBox(), wxID_ANY);
    _braceBadlightStyleBackgroundCtrl = new wxColourPickerCtrl(braceBadlightStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* braceBadlightStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    braceBadlightStyleColorBox->Add(new wxStaticText(braceBadlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    braceBadlightStyleColorBox->Add(_braceBadlightStyleForegroundCtrl);

    braceBadlightStyleColorBox->Add(new wxStaticText(braceBadlightStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    braceBadlightStyleColorBox->Add(_braceBadlightStyleBackgroundCtrl);

    braceBadlightStyleBox->Add(braceBadlightStyleBoldItalicBox, 0, wxALL, 5);
    braceBadlightStyleBox->Add(braceBadlightStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    panelSizer->Add(_enableBraceMatchingCtrl, 0, wxEXPAND | wxALL, 5);
    panelSizer->AddSpacer(10);
    panelSizer->Add(braceHighlightStyleBox,   0, wxEXPAND | wxALL, 5);
    panelSizer->AddSpacer(10);
    panelSizer->Add(braceBadlightStyleBox,    0, wxEXPAND | wxALL, 5);

    panel->SetSizer(panelSizer);

    return panel;
}

//-------------------------------------------------------------------
wxWindow*
PreferencesDialog::createTextEditorSyntaxPage(wxWindow* parentCtrl)
{
    wxPanel* panel = new wxPanel(parentCtrl, wxID_ANY);

    _enableSyntaxHighlightingCtrl = new wxCheckBox(panel, wxID_ANY, wxT("Enable syntax highlighting"));

    wxListbook* panelContainer = new wxListbook(panel, wxID_ANY);

    // operator
    wxPanel* operatorPanel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* operatorStyleBox = new wxStaticBoxSizer(wxVERTICAL, operatorPanel, wxT("Operator style"));

    _operatorStyleBoldCtrl   = new wxCheckBox(operatorStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _operatorStyleItalicCtrl = new wxCheckBox(operatorStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* operatorStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    operatorStyleBoldItalicBox->Add(_operatorStyleBoldCtrl);
    operatorStyleBoldItalicBox->AddSpacer(10);
    operatorStyleBoldItalicBox->Add(_operatorStyleItalicCtrl);

    _operatorStyleForegroundCtrl = new wxColourPickerCtrl(operatorStyleBox->GetStaticBox(), wxID_ANY);
    _operatorStyleBackgroundCtrl = new wxColourPickerCtrl(operatorStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* operatorStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    operatorStyleColorBox->Add(new wxStaticText(operatorStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    operatorStyleColorBox->Add(_operatorStyleForegroundCtrl);

    operatorStyleColorBox->Add(new wxStaticText(operatorStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    operatorStyleColorBox->Add(_operatorStyleBackgroundCtrl);

    operatorStyleBox->Add(operatorStyleBoldItalicBox, 0, wxALL, 5);
    operatorStyleBox->Add(operatorStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* operatorPanelSizer = new wxBoxSizer(wxVERTICAL);

    operatorPanelSizer->Add(operatorStyleBox, 0, wxEXPAND | wxALL, 5);

    operatorPanel->SetSizer(operatorPanelSizer);

    // string
    wxPanel* stringPanel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* stringStyleBox = new wxStaticBoxSizer(wxVERTICAL, stringPanel, wxT("String style"));

    _stringStyleBoldCtrl   = new wxCheckBox(stringStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _stringStyleItalicCtrl = new wxCheckBox(stringStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* stringStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    stringStyleBoldItalicBox->Add(_stringStyleBoldCtrl);
    stringStyleBoldItalicBox->AddSpacer(10);
    stringStyleBoldItalicBox->Add(_stringStyleItalicCtrl);

    _stringStyleForegroundCtrl = new wxColourPickerCtrl(stringStyleBox->GetStaticBox(), wxID_ANY);
    _stringStyleBackgroundCtrl = new wxColourPickerCtrl(stringStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* stringStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    stringStyleColorBox->Add(new wxStaticText(stringStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    stringStyleColorBox->Add(_stringStyleForegroundCtrl);

    stringStyleColorBox->Add(new wxStaticText(stringStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    stringStyleColorBox->Add(_stringStyleBackgroundCtrl);

    stringStyleBox->Add(stringStyleBoldItalicBox, 0, wxALL, 5);
    stringStyleBox->Add(stringStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* stringPanelSizer = new wxBoxSizer(wxVERTICAL);

    stringPanelSizer->Add(stringStyleBox, 0, wxEXPAND | wxALL, 5);

    stringPanel->SetSizer(stringPanelSizer);

    // identifier
    wxPanel* identifierPanel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* identifierStyleBox = new wxStaticBoxSizer(wxVERTICAL, identifierPanel, wxT("Identifier style"));

    _identifierStyleBoldCtrl   = new wxCheckBox(identifierStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _identifierStyleItalicCtrl = new wxCheckBox(identifierStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* identifierStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    identifierStyleBoldItalicBox->Add(_identifierStyleBoldCtrl);
    identifierStyleBoldItalicBox->AddSpacer(10);
    identifierStyleBoldItalicBox->Add(_identifierStyleItalicCtrl);

    _identifierStyleForegroundCtrl = new wxColourPickerCtrl(identifierStyleBox->GetStaticBox(), wxID_ANY);
    _identifierStyleBackgroundCtrl = new wxColourPickerCtrl(identifierStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* identifierStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    identifierStyleColorBox->Add(new wxStaticText(identifierStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    identifierStyleColorBox->Add(_identifierStyleForegroundCtrl);

    identifierStyleColorBox->Add(new wxStaticText(identifierStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    identifierStyleColorBox->Add(_identifierStyleBackgroundCtrl);

    identifierStyleBox->Add(identifierStyleBoldItalicBox, 0, wxALL, 5);
    identifierStyleBox->Add(identifierStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* identifierPanelSizer = new wxBoxSizer(wxVERTICAL);

    identifierPanelSizer->Add(identifierStyleBox, 0, wxEXPAND | wxALL, 5);

    identifierPanel->SetSizer(identifierPanelSizer);

    // number
    wxPanel* numberPanel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* numberStyleBox = new wxStaticBoxSizer(wxVERTICAL, numberPanel, wxT("Number style"));

    _numberStyleBoldCtrl   = new wxCheckBox(numberStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _numberStyleItalicCtrl = new wxCheckBox(numberStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* numberStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    numberStyleBoldItalicBox->Add(_numberStyleBoldCtrl);
    numberStyleBoldItalicBox->AddSpacer(10);
    numberStyleBoldItalicBox->Add(_numberStyleItalicCtrl);

    _numberStyleForegroundCtrl = new wxColourPickerCtrl(numberStyleBox->GetStaticBox(), wxID_ANY);
    _numberStyleBackgroundCtrl = new wxColourPickerCtrl(numberStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* numberStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    numberStyleColorBox->Add(new wxStaticText(numberStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    numberStyleColorBox->Add(_numberStyleForegroundCtrl);

    numberStyleColorBox->Add(new wxStaticText(numberStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    numberStyleColorBox->Add(_numberStyleBackgroundCtrl);

    numberStyleBox->Add(numberStyleBoldItalicBox, 0, wxALL, 5);
    numberStyleBox->Add(numberStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* numberPanelSizer = new wxBoxSizer(wxVERTICAL);

    numberPanelSizer->Add(numberStyleBox, 0, wxEXPAND | wxALL, 5);

    numberPanel->SetSizer(numberPanelSizer);

    // character
    wxPanel* characterPanel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* characterStyleBox = new wxStaticBoxSizer(wxVERTICAL, characterPanel, wxT("Character style"));

    _characterStyleBoldCtrl   = new wxCheckBox(characterStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _characterStyleItalicCtrl = new wxCheckBox(characterStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* characterStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    characterStyleBoldItalicBox->Add(_characterStyleBoldCtrl);
    characterStyleBoldItalicBox->AddSpacer(10);
    characterStyleBoldItalicBox->Add(_characterStyleItalicCtrl);

    _characterStyleForegroundCtrl = new wxColourPickerCtrl(characterStyleBox->GetStaticBox(), wxID_ANY);
    _characterStyleBackgroundCtrl = new wxColourPickerCtrl(characterStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* characterStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    characterStyleColorBox->Add(new wxStaticText(characterStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    characterStyleColorBox->Add(_characterStyleForegroundCtrl);

    characterStyleColorBox->Add(new wxStaticText(characterStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    characterStyleColorBox->Add(_characterStyleBackgroundCtrl);

    characterStyleBox->Add(characterStyleBoldItalicBox, 0, wxALL, 5);
    characterStyleBox->Add(characterStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* characterPanelSizer = new wxBoxSizer(wxVERTICAL);

    characterPanelSizer->Add(characterStyleBox, 0, wxEXPAND | wxALL, 5);

    characterPanel->SetSizer(characterPanelSizer);

    // comment
    wxPanel* commentPanel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* commentStyleBox = new wxStaticBoxSizer(wxVERTICAL, commentPanel, wxT("Comment style"));

    _commentStyleBoldCtrl   = new wxCheckBox(commentStyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _commentStyleItalicCtrl = new wxCheckBox(commentStyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* commentStyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    commentStyleBoldItalicBox->Add(_commentStyleBoldCtrl);
    commentStyleBoldItalicBox->AddSpacer(10);
    commentStyleBoldItalicBox->Add(_commentStyleItalicCtrl);

    _commentStyleForegroundCtrl = new wxColourPickerCtrl(commentStyleBox->GetStaticBox(), wxID_ANY);
    _commentStyleBackgroundCtrl = new wxColourPickerCtrl(commentStyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* commentStyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    commentStyleColorBox->Add(new wxStaticText(commentStyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    commentStyleColorBox->Add(_commentStyleForegroundCtrl);

    commentStyleColorBox->Add(new wxStaticText(commentStyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    commentStyleColorBox->Add(_commentStyleBackgroundCtrl);

    commentStyleBox->Add(commentStyleBoldItalicBox, 0, wxALL, 5);
    commentStyleBox->Add(commentStyleColorBox,      0, wxALL, 5);

    wxBoxSizer* commentPanelSizer = new wxBoxSizer(wxVERTICAL);

    commentPanelSizer->Add(commentStyleBox, 0, wxEXPAND | wxALL, 5);

    commentPanel->SetSizer(commentPanelSizer);

    // keyword 1
    wxPanel* keyword1Panel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* keyword1StyleBox = new wxStaticBoxSizer(wxVERTICAL, keyword1Panel, wxT("Keyword 1 style"));

    _keyword1StyleBoldCtrl   = new wxCheckBox(keyword1StyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _keyword1StyleItalicCtrl = new wxCheckBox(keyword1StyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* keyword1StyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    keyword1StyleBoldItalicBox->Add(_keyword1StyleBoldCtrl);
    keyword1StyleBoldItalicBox->AddSpacer(10);
    keyword1StyleBoldItalicBox->Add(_keyword1StyleItalicCtrl);

    _keyword1StyleForegroundCtrl = new wxColourPickerCtrl(keyword1StyleBox->GetStaticBox(), wxID_ANY);
    _keyword1StyleBackgroundCtrl = new wxColourPickerCtrl(keyword1StyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* keyword1StyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    keyword1StyleColorBox->Add(new wxStaticText(keyword1StyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    keyword1StyleColorBox->Add(_keyword1StyleForegroundCtrl);

    keyword1StyleColorBox->Add(new wxStaticText(keyword1StyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    keyword1StyleColorBox->Add(_keyword1StyleBackgroundCtrl);

    keyword1StyleBox->Add(keyword1StyleBoldItalicBox, 0, wxALL, 5);
    keyword1StyleBox->Add(keyword1StyleColorBox,      0, wxALL, 5);

    _keyword1List = new wxEditableListBox(keyword1Panel, wxID_ANY, wxT("Keyword 1 list"), wxDefaultPosition, wxDefaultSize, wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);

    wxBoxSizer* keyword1PanelSizer = new wxBoxSizer(wxVERTICAL);

    keyword1PanelSizer->Add(keyword1StyleBox, 0, wxEXPAND | wxALL, 5);
    keyword1PanelSizer->AddSpacer(10);
    keyword1PanelSizer->Add(_keyword1List,    1, wxEXPAND | wxALL, 5);

    keyword1Panel->SetSizer(keyword1PanelSizer);

    // keyword 2
    wxPanel* keyword2Panel = new wxPanel(panelContainer, wxID_ANY);

    wxStaticBoxSizer* keyword2StyleBox = new wxStaticBoxSizer(wxVERTICAL, keyword2Panel, wxT("Keyword 2 style"));

    _keyword2StyleBoldCtrl   = new wxCheckBox(keyword2StyleBox->GetStaticBox(), wxID_ANY, wxT("Bold"));
    _keyword2StyleItalicCtrl = new wxCheckBox(keyword2StyleBox->GetStaticBox(), wxID_ANY, wxT("Italic"));

    wxBoxSizer* keyword2StyleBoldItalicBox = new wxBoxSizer(wxHORIZONTAL);

    keyword2StyleBoldItalicBox->Add(_keyword2StyleBoldCtrl);
    keyword2StyleBoldItalicBox->AddSpacer(10);
    keyword2StyleBoldItalicBox->Add(_keyword2StyleItalicCtrl);

    _keyword2StyleForegroundCtrl = new wxColourPickerCtrl(keyword2StyleBox->GetStaticBox(), wxID_ANY);
    _keyword2StyleBackgroundCtrl = new wxColourPickerCtrl(keyword2StyleBox->GetStaticBox(), wxID_ANY);

    wxFlexGridSizer* keyword2StyleColorBox = new wxFlexGridSizer(2, 2, 5, 5);

    keyword2StyleColorBox->Add(new wxStaticText(keyword2StyleBox->GetStaticBox(), wxID_ANY, wxT("Foreground:")));
    keyword2StyleColorBox->Add(_keyword2StyleForegroundCtrl);

    keyword2StyleColorBox->Add(new wxStaticText(keyword2StyleBox->GetStaticBox(), wxID_ANY, wxT("Background:")));
    keyword2StyleColorBox->Add(_keyword2StyleBackgroundCtrl);

    keyword2StyleBox->Add(keyword2StyleBoldItalicBox, 0, wxALL, 5);
    keyword2StyleBox->Add(keyword2StyleColorBox,      0, wxALL, 5);

    _keyword2List = new wxEditableListBox(keyword2Panel, wxID_ANY, wxT("Keyword 2 list"), wxDefaultPosition, wxDefaultSize, wxEL_DEFAULT_STYLE | wxEL_NO_REORDER);

    wxBoxSizer* keyword2PanelSizer = new wxBoxSizer(wxVERTICAL);

    keyword2PanelSizer->Add(keyword2StyleBox, 0, wxEXPAND | wxALL, 5);
    keyword2PanelSizer->AddSpacer(10);
    keyword2PanelSizer->Add(_keyword2List,    1, wxEXPAND | wxALL, 5);

    keyword2Panel->SetSizer(keyword2PanelSizer);

    panelContainer->AddPage(identifierPanel, wxT("Identifier"));
    panelContainer->AddPage(numberPanel,     wxT("Number"));
    panelContainer->AddPage(characterPanel,  wxT("Character"));
    panelContainer->AddPage(stringPanel,     wxT("String"));
    panelContainer->AddPage(commentPanel,    wxT("Comment"));
    panelContainer->AddPage(operatorPanel,   wxT("Operator"));
    panelContainer->AddPage(keyword1Panel,   wxT("Keyword 1"));
    panelContainer->AddPage(keyword2Panel,   wxT("Keyword 2"));

    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    panelSizer->Add(_enableSyntaxHighlightingCtrl, 0,            wxALL, 5);
    panelSizer->AddSpacer(10);
    panelSizer->Add(panelContainer,                1, wxEXPAND | wxALL, 5);

    panel->SetSizer(panelSizer);

    return panel;
}

//-------------------------------------------------------------------
void
PreferencesDialog::onRestoreDefaults(wxCommandEvent& event)
{
    _preferences.loadDefaults();

    // update controls
    loadValues();
}

//-------------------------------------------------------------------
void
PreferencesDialog::onOk(wxCommandEvent& event)
{
    saveValues();

    EndModal(wxID_OK);
}

//-------------------------------------------------------------------
void
PreferencesDialog::onCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

//-------------------------------------------------------------------
void
PreferencesDialog::onDataLocationPicked(wxFileDirPickerEvent& event)
{
    _dataLocationCtrl->SetValue(event.GetPath());
}

//-------------------------------------------------------------------
void
PreferencesDialog::onCommonDataLocationPicked(wxFileDirPickerEvent& event)
{
    _commonDataLocationCtrl->SetValue(event.GetPath());
}
