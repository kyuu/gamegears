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

#ifndef PREFERENCESDIALOG_HPP
#define PREFERENCESDIALOG_HPP

#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/fontpicker.h>
#include <wx/clrpicker.h>
#include <wx/editlbox.h>
#include <wx/listbook.h>
#include <wx/spinctrl.h>
#include "Preferences.hpp"


class PreferencesDialog : public wxDialog {
public:
    enum {
        ID_RESTORE_DEFAULTS = 0,
        ID_BROWSE_DATA_LOCATION,
        ID_BROWSE_COMMON_DATA_LOCATION,
    };

    explicit PreferencesDialog(const Preferences& initialPreferences, const wxString& initialPath);
    ~PreferencesDialog();

    const Preferences& getPreferences() const;

private:
    void loadValues();
    void saveValues();

    wxWindow* createDeveloperGeneralPage(wxWindow* parentCtrl);
    wxWindow* createTextEditorGeneralPage(wxWindow* parentCtrl);
    wxWindow* createTextEditorIndentPage(wxWindow* parentCtrl);
    wxWindow* createTextEditorMarginPage(wxWindow* parentCtrl);
    wxWindow* createTextEditorBracesPage(wxWindow* parentCtrl);
    wxWindow* createTextEditorSyntaxPage(wxWindow* parentCtrl);

    void onRestoreDefaults(wxCommandEvent& event);
    void onOk(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    void onDataLocationPicked(wxFileDirPickerEvent& event);
    void onCommonDataLocationPicked(wxFileDirPickerEvent& event);

private:
    Preferences _preferences;
    wxString _initialPath;

    // developer general
    wxTextCtrl* _dataLocationCtrl;
    wxTextCtrl* _commonDataLocationCtrl;
    wxCheckBox* _openLastProjectCtrl;

    wxEditableListBox* _projectFileExtensionList;
    wxEditableListBox* _scriptFileExtensionList;
    wxEditableListBox* _textFileExtensionList;

    // text editor general
    wxFontPickerCtrl* _fontCtrl;

    wxSpinCtrl* _initialZoomLevelCtrl;

    //wxCheckBox* _enableAutoCompleteCtrl;
    //wxCheckBox* _enableCallTipsCtrl;

    // text editor margin
    wxCheckBox* _displayLineNumbersCtrl;

    wxCheckBox* _lineNumberStyleBoldCtrl;
    wxCheckBox* _lineNumberStyleItalicCtrl;

    wxColourPickerCtrl* _lineNumberStyleForegroundCtrl;
    wxColourPickerCtrl* _lineNumberStyleBackgroundCtrl;

    wxCheckBox* _displayFoldersCtrl;
    wxCheckBox* _foldAllOnStartupCtrl;

    wxChoice*           _folderHeaderStyleShapeCtrl;
    wxColourPickerCtrl* _folderHeaderStyleForegroundCtrl;
    wxColourPickerCtrl* _folderHeaderStyleBackgroundCtrl;

    wxChoice*           _folderMidHeaderStyleShapeCtrl;
    wxColourPickerCtrl* _folderMidHeaderStyleForegroundCtrl;
    wxColourPickerCtrl* _folderMidHeaderStyleBackgroundCtrl;

    wxChoice*           _folderBodyStyleShapeCtrl;
    wxColourPickerCtrl* _folderBodyStyleForegroundCtrl;
    wxColourPickerCtrl* _folderBodyStyleBackgroundCtrl;

    // text editor indent
    wxSpinCtrl* _tabSizeCtrl;
    wxCheckBox* _convertTabsToSpacesCtrl;

    wxSpinCtrl* _indentSizeCtrl;
    wxCheckBox* _enableAutoIndentCtrl;
    wxCheckBox* _backspaceUnindentsCtrl;
    wxCheckBox* _showIndentationGuidesCtrl;

    // text editor braces
    wxCheckBox* _enableBraceMatchingCtrl;

    wxCheckBox* _braceHighlightStyleBoldCtrl;
    wxCheckBox* _braceHighlightStyleItalicCtrl;

    wxColourPickerCtrl* _braceHighlightStyleForegroundCtrl;
    wxColourPickerCtrl* _braceHighlightStyleBackgroundCtrl;

    wxCheckBox* _braceBadlightStyleBoldCtrl;
    wxCheckBox* _braceBadlightStyleItalicCtrl;

    wxColourPickerCtrl* _braceBadlightStyleForegroundCtrl;
    wxColourPickerCtrl* _braceBadlightStyleBackgroundCtrl;

    // text editor syntax
    wxCheckBox* _enableSyntaxHighlightingCtrl;

    wxCheckBox* _operatorStyleBoldCtrl;
    wxCheckBox* _operatorStyleItalicCtrl;

    wxColourPickerCtrl* _operatorStyleForegroundCtrl;
    wxColourPickerCtrl* _operatorStyleBackgroundCtrl;

    wxCheckBox* _stringStyleBoldCtrl;
    wxCheckBox* _stringStyleItalicCtrl;

    wxColourPickerCtrl* _stringStyleForegroundCtrl;
    wxColourPickerCtrl* _stringStyleBackgroundCtrl;

    wxCheckBox* _identifierStyleBoldCtrl;
    wxCheckBox* _identifierStyleItalicCtrl;

    wxColourPickerCtrl* _identifierStyleForegroundCtrl;
    wxColourPickerCtrl* _identifierStyleBackgroundCtrl;

    wxCheckBox* _numberStyleBoldCtrl;
    wxCheckBox* _numberStyleItalicCtrl;

    wxColourPickerCtrl* _numberStyleForegroundCtrl;
    wxColourPickerCtrl* _numberStyleBackgroundCtrl;

    wxCheckBox* _characterStyleBoldCtrl;
    wxCheckBox* _characterStyleItalicCtrl;

    wxColourPickerCtrl* _characterStyleForegroundCtrl;
    wxColourPickerCtrl* _characterStyleBackgroundCtrl;

    wxCheckBox* _commentStyleBoldCtrl;
    wxCheckBox* _commentStyleItalicCtrl;

    wxColourPickerCtrl* _commentStyleForegroundCtrl;
    wxColourPickerCtrl* _commentStyleBackgroundCtrl;

    wxCheckBox* _keyword1StyleBoldCtrl;
    wxCheckBox* _keyword1StyleItalicCtrl;

    wxColourPickerCtrl* _keyword1StyleForegroundCtrl;
    wxColourPickerCtrl* _keyword1StyleBackgroundCtrl;

    wxEditableListBox* _keyword1List;

    wxCheckBox* _keyword2StyleBoldCtrl;
    wxCheckBox* _keyword2StyleItalicCtrl;

    wxColourPickerCtrl* _keyword2StyleForegroundCtrl;
    wxColourPickerCtrl* _keyword2StyleBackgroundCtrl;

    wxEditableListBox* _keyword2List;
};


#endif
