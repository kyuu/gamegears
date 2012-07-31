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

#ifndef TEXTEDITOR_HPP
#define TEXTEDITOR_HPP

#include <vector>
#include <string>
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include "FileEditorDelegator.hpp"


// forward declaration
class Developer;


class TextEditor : public wxStyledTextCtrl {
public:
    TextEditor(Developer* developer, wxWindow* parent, const wxString& fileName, bool scriptMode = false);
    ~TextEditor();

    // event handlers
    void onMarginClick(wxStyledTextEvent& event);
    void onUpdateUI(wxStyledTextEvent& event);
    void onCharAdded(wxStyledTextEvent& event);
    void onSavePointReached(wxStyledTextEvent& event);
    void onSavePointLeft(wxStyledTextEvent& event);

    // invoked by the delegator
    int getFileEditorType() const;
    const wxString& getFileName() const;
    bool isFileModified() const;
    bool saveFile();
    bool saveFileAs(const wxString& fileName = wxEmptyString);
    bool canDoCommand(int cmd) const;
    void doCommand(int cmd);

private:
    void foldAll();
    wxString getEOLString() const;

private:
    FileEditorDelegator<TextEditor> _delegator;
    Developer* _developer;

    bool _scriptMode;
    bool _displayLineNumbers;
    bool _autoIndent;
    bool _matchBraces;
    bool _autoComplete;
    bool _showCallTips;

    wxString _fileName;
};


#endif
