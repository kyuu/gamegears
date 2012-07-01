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

#include <algorithm>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/fontenum.h>
#include "TextEditor.hpp"
#include "Developer.hpp"
#include "commands.hpp"

#define MARGIN_LINENUMBER 0
#define MARGIN_FOLD       1

#define MARGIN_LINENUMBER_WIDTH_BASE       40
#define MARGIN_LINENUMBER_WIDTH_MULTIPLIER  5


//-------------------------------------------------------------------
TextEditor::TextEditor(Developer* developer, wxWindow* parent, const wxString& fileName, bool scriptMode)
    : wxStyledTextCtrl(parent, wxID_ANY)
    , _delegator(this)
    , _developer(developer)
    , _scriptMode(scriptMode)
    , _displayLineNumbers(false)
    , _autoIndent(true)
    , _matchBraces(true)
    , _autoComplete(false)
    , _showCallTips(false)
    , _fileName(fileName)
{
    SetClientData(&_delegator);

    const Preferences& prefs = _developer->getPreferences();

    // set zoom level
    SetZoom(prefs.TextEditor.General.InitialZoomLevel);

    // set font
    wxFont font;
    font.SetFaceName(prefs.TextEditor.General.Font.FaceName);
    font.SetPointSize(prefs.TextEditor.General.Font.PointSize);
    StyleSetFont(wxSTC_STYLE_DEFAULT, font);

    // clear all styles to defaults
    StyleClearAll();

    if (_scriptMode) {
        // setup indentation
        SetTabIndents(true);
        SetTabWidth(prefs.TextEditor.Indent.TabSize);
        SetUseTabs(prefs.TextEditor.Indent.ConvertTabsToSpaces == false);

        SetIndent(prefs.TextEditor.Indent.IndentSize);
        SetIndentationGuides(prefs.TextEditor.Indent.ShowIndentationGuides);
        SetBackSpaceUnIndents(prefs.TextEditor.Indent.BackspaceUnindents);
        _autoIndent = prefs.TextEditor.Indent.EnableAutoIndent;

        // setup line numbers margin
        _displayLineNumbers = prefs.TextEditor.Margin.DisplayLineNumbers;
        if (prefs.TextEditor.Margin.DisplayLineNumbers) {
            SetMarginType(MARGIN_LINENUMBER, wxSTC_MARGIN_NUMBER);
            SetMarginWidth(MARGIN_LINENUMBER, MARGIN_LINENUMBER_WIDTH_BASE + (prefs.TextEditor.General.InitialZoomLevel > 0 ? prefs.TextEditor.General.InitialZoomLevel * MARGIN_LINENUMBER_WIDTH_MULTIPLIER : 0));
            SetMarginMask(MARGIN_LINENUMBER, ~wxSTC_MASK_FOLDERS);

            StyleSetForeground(wxSTC_STYLE_LINENUMBER, prefs.TextEditor.Margin.LineNumberStyle.Foreground);
            StyleSetBackground(wxSTC_STYLE_LINENUMBER, prefs.TextEditor.Margin.LineNumberStyle.Background);
            StyleSetBold      (wxSTC_STYLE_LINENUMBER, prefs.TextEditor.Margin.LineNumberStyle.Bold);
            StyleSetItalic    (wxSTC_STYLE_LINENUMBER, prefs.TextEditor.Margin.LineNumberStyle.Italic);
        }

        // setup folding and folders margin
        if (prefs.TextEditor.Margin.DisplayFolders) {
            SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
            SetMarginWidth(MARGIN_FOLD, 16);
            SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
            SetMarginSensitive(MARGIN_FOLD, true);

            SetProperty(wxT("fold"),         wxT("1"));
            SetProperty(wxT("fold.comment"), wxT("1"));
            SetProperty(wxT("fold.compact"), wxT("0"));

            switch (prefs.TextEditor.Margin.FolderHeaderStyle.Shape) {
                case TE_FOLDER_HEADER_STYLE_ARROW:
                    MarkerDefine(wxSTC_MARKNUM_FOLDER,     wxSTC_MARK_ARROWDOWN);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROW);
                    break;

                case TE_FOLDER_HEADER_STYLE_PLUS_MINUS:
                    MarkerDefine(wxSTC_MARKNUM_FOLDER,     wxSTC_MARK_PLUS);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS);
                    break;

                case TE_FOLDER_HEADER_STYLE_PLUS_MINUS_BOX:
                    MarkerDefine(wxSTC_MARKNUM_FOLDER,     wxSTC_MARK_BOXPLUS);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
                    break;

                case TE_FOLDER_HEADER_STYLE_PLUS_MINUS_CIRCLE:
                    MarkerDefine(wxSTC_MARKNUM_FOLDER,     wxSTC_MARK_CIRCLEPLUS);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_CIRCLEMINUS);
                    break;

                default:
                    MarkerDefine(wxSTC_MARKNUM_FOLDER,     wxSTC_MARK_ARROWDOWN);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROW);
                    break;
            }

            MarkerSetForeground(wxSTC_MARKNUM_FOLDER,     prefs.TextEditor.Margin.FolderHeaderStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDER,     prefs.TextEditor.Margin.FolderHeaderStyle.Background);
            MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, prefs.TextEditor.Margin.FolderHeaderStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, prefs.TextEditor.Margin.FolderHeaderStyle.Background);

            switch (prefs.TextEditor.Margin.FolderMidHeaderStyle.Shape) {
                case TE_FOLDER_HEADER_STYLE_ARROW:
                    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_ARROWDOWN);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROW);
                    break;

                case TE_FOLDER_HEADER_STYLE_PLUS_MINUS:
                    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_PLUS);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_MINUS);
                    break;

                case TE_FOLDER_HEADER_STYLE_PLUS_MINUS_BOX:
                    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_BOXPLUSCONNECTED);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED);
                    break;

                case TE_FOLDER_HEADER_STYLE_PLUS_MINUS_CIRCLE:
                    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_CIRCLEPLUSCONNECTED);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_CIRCLEMINUSCONNECTED);
                    break;

                default:
                    MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_EMPTY);
                    MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_EMPTY);
                    break;
            }

            MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND,     prefs.TextEditor.Margin.FolderMidHeaderStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND,     prefs.TextEditor.Margin.FolderMidHeaderStyle.Background);
            MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, prefs.TextEditor.Margin.FolderMidHeaderStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, prefs.TextEditor.Margin.FolderMidHeaderStyle.Background);

            switch (prefs.TextEditor.Margin.FolderBodyStyle.Shape) {
                case TE_FOLDER_BODY_STYLE_EMPTY:
                    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_EMPTY);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_EMPTY);
                    break;

                case TE_FOLDER_BODY_STYLE_LINE:
                    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER);
                    break;

                case TE_FOLDER_BODY_STYLE_CURVED_LINE:
                    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNERCURVE);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNERCURVE);
                    break;

                default:
                    MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNERCURVE);
                    MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNERCURVE);
                    break;
            }

            MarkerSetForeground(wxSTC_MARKNUM_FOLDERSUB,     prefs.TextEditor.Margin.FolderBodyStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDERSUB,     prefs.TextEditor.Margin.FolderBodyStyle.Background);
            MarkerSetForeground(wxSTC_MARKNUM_FOLDERTAIL,    prefs.TextEditor.Margin.FolderBodyStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDERTAIL,    prefs.TextEditor.Margin.FolderBodyStyle.Background);
            MarkerSetForeground(wxSTC_MARKNUM_FOLDERMIDTAIL, prefs.TextEditor.Margin.FolderBodyStyle.Foreground);
            MarkerSetBackground(wxSTC_MARKNUM_FOLDERMIDTAIL, prefs.TextEditor.Margin.FolderBodyStyle.Background);

        } else {
            SetMarginWidth(MARGIN_FOLD, 0);
            SetProperty(wxT("fold"), wxT("0"));
        }

        // setup brace matching
        _matchBraces = prefs.TextEditor.Braces.EnableBraceMatching;

        StyleSetForeground(wxSTC_STYLE_BRACELIGHT, prefs.TextEditor.Braces.HighlightStyle.Foreground);
        StyleSetBackground(wxSTC_STYLE_BRACELIGHT, prefs.TextEditor.Braces.HighlightStyle.Background);
        StyleSetBold      (wxSTC_STYLE_BRACELIGHT, prefs.TextEditor.Braces.HighlightStyle.Bold);
        StyleSetItalic    (wxSTC_STYLE_BRACELIGHT, prefs.TextEditor.Braces.HighlightStyle.Italic);

        StyleSetForeground(wxSTC_STYLE_BRACEBAD, prefs.TextEditor.Braces.BadlightStyle.Foreground);
        StyleSetBackground(wxSTC_STYLE_BRACEBAD, prefs.TextEditor.Braces.BadlightStyle.Background);
        StyleSetBold      (wxSTC_STYLE_BRACEBAD, prefs.TextEditor.Braces.BadlightStyle.Bold);
        StyleSetItalic    (wxSTC_STYLE_BRACEBAD, prefs.TextEditor.Braces.BadlightStyle.Italic);

        // setup autocomplete
        _autoComplete = prefs.TextEditor.General.EnableAutoComplete;

        // setup calltips
        _showCallTips = prefs.TextEditor.General.EnableCallTips;

        if (prefs.TextEditor.Syntax.EnableSyntaxHighlighting) {
            // set lexer
            SetLexer(wxSTC_LEX_CPP);

            // set string style
            StyleSetForeground(wxSTC_C_STRING, prefs.TextEditor.Syntax.String.Style.Foreground);
            StyleSetBackground(wxSTC_C_STRING, prefs.TextEditor.Syntax.String.Style.Background);
            StyleSetBold      (wxSTC_C_STRING, prefs.TextEditor.Syntax.String.Style.Bold);
            StyleSetItalic    (wxSTC_C_STRING, prefs.TextEditor.Syntax.String.Style.Italic);

            // set identifier style
            StyleSetForeground(wxSTC_C_IDENTIFIER, prefs.TextEditor.Syntax.Identifier.Style.Foreground);
            StyleSetBackground(wxSTC_C_IDENTIFIER, prefs.TextEditor.Syntax.Identifier.Style.Background);
            StyleSetBold      (wxSTC_C_IDENTIFIER, prefs.TextEditor.Syntax.Identifier.Style.Bold);
            StyleSetItalic    (wxSTC_C_IDENTIFIER, prefs.TextEditor.Syntax.Identifier.Style.Italic);

            // set number style
            StyleSetForeground(wxSTC_C_NUMBER, prefs.TextEditor.Syntax.Number.Style.Foreground);
            StyleSetBackground(wxSTC_C_NUMBER, prefs.TextEditor.Syntax.Number.Style.Background);
            StyleSetBold      (wxSTC_C_NUMBER, prefs.TextEditor.Syntax.Number.Style.Bold);
            StyleSetItalic    (wxSTC_C_NUMBER, prefs.TextEditor.Syntax.Number.Style.Italic);

            // set character style
            StyleSetForeground(wxSTC_C_CHARACTER, prefs.TextEditor.Syntax.Character.Style.Foreground);
            StyleSetBackground(wxSTC_C_CHARACTER, prefs.TextEditor.Syntax.Character.Style.Background);
            StyleSetBold      (wxSTC_C_CHARACTER, prefs.TextEditor.Syntax.Character.Style.Bold);
            StyleSetItalic    (wxSTC_C_CHARACTER, prefs.TextEditor.Syntax.Character.Style.Italic);

            // set comment style
            StyleSetForeground(wxSTC_C_PREPROCESSOR,           prefs.TextEditor.Syntax.Comment.Style.Foreground);
            StyleSetForeground(wxSTC_C_COMMENT,                prefs.TextEditor.Syntax.Comment.Style.Foreground);
            StyleSetForeground(wxSTC_C_COMMENTLINE,            prefs.TextEditor.Syntax.Comment.Style.Foreground);
            StyleSetForeground(wxSTC_C_COMMENTDOC,             prefs.TextEditor.Syntax.Comment.Style.Foreground);
            StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD,      prefs.TextEditor.Syntax.Comment.Style.Foreground);
            StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, prefs.TextEditor.Syntax.Comment.Style.Foreground);

            StyleSetBackground(wxSTC_C_PREPROCESSOR,           prefs.TextEditor.Syntax.Comment.Style.Background);
            StyleSetBackground(wxSTC_C_COMMENT,                prefs.TextEditor.Syntax.Comment.Style.Background);
            StyleSetBackground(wxSTC_C_COMMENTLINE,            prefs.TextEditor.Syntax.Comment.Style.Background);
            StyleSetBackground(wxSTC_C_COMMENTDOC,             prefs.TextEditor.Syntax.Comment.Style.Background);
            StyleSetBackground(wxSTC_C_COMMENTDOCKEYWORD,      prefs.TextEditor.Syntax.Comment.Style.Background);
            StyleSetBackground(wxSTC_C_COMMENTDOCKEYWORDERROR, prefs.TextEditor.Syntax.Comment.Style.Background);

            StyleSetBold      (wxSTC_C_PREPROCESSOR,           prefs.TextEditor.Syntax.Comment.Style.Bold);
            StyleSetBold      (wxSTC_C_COMMENT,                prefs.TextEditor.Syntax.Comment.Style.Bold);
            StyleSetBold      (wxSTC_C_COMMENTLINE,            prefs.TextEditor.Syntax.Comment.Style.Bold);
            StyleSetBold      (wxSTC_C_COMMENTDOC,             prefs.TextEditor.Syntax.Comment.Style.Bold);
            StyleSetBold      (wxSTC_C_COMMENTDOCKEYWORD,      prefs.TextEditor.Syntax.Comment.Style.Bold);
            StyleSetBold      (wxSTC_C_COMMENTDOCKEYWORDERROR, prefs.TextEditor.Syntax.Comment.Style.Bold);

            StyleSetItalic    (wxSTC_C_PREPROCESSOR,           prefs.TextEditor.Syntax.Comment.Style.Italic);
            StyleSetItalic    (wxSTC_C_COMMENT,                prefs.TextEditor.Syntax.Comment.Style.Italic);
            StyleSetItalic    (wxSTC_C_COMMENTLINE,            prefs.TextEditor.Syntax.Comment.Style.Italic);
            StyleSetItalic    (wxSTC_C_COMMENTDOC,             prefs.TextEditor.Syntax.Comment.Style.Italic);
            StyleSetItalic    (wxSTC_C_COMMENTDOCKEYWORD,      prefs.TextEditor.Syntax.Comment.Style.Italic);
            StyleSetItalic    (wxSTC_C_COMMENTDOCKEYWORDERROR, prefs.TextEditor.Syntax.Comment.Style.Italic);

            // set operator style
            StyleSetForeground(wxSTC_C_OPERATOR, prefs.TextEditor.Syntax.Operator.Style.Foreground);
            StyleSetBackground(wxSTC_C_OPERATOR, prefs.TextEditor.Syntax.Operator.Style.Background);
            StyleSetBold      (wxSTC_C_OPERATOR, prefs.TextEditor.Syntax.Operator.Style.Bold);
            StyleSetItalic    (wxSTC_C_OPERATOR, prefs.TextEditor.Syntax.Operator.Style.Italic);

            // set keyword 1 style
            const wxArrayString& kw1 = prefs.TextEditor.Syntax.Keyword1.List;
            wxString kw1Str;
            for (size_t i = 0; i < kw1.GetCount(); i++) {
                kw1Str += kw1[i] + " ";
            }
            SetKeyWords(0, kw1Str);

            StyleSetForeground(wxSTC_C_WORD, prefs.TextEditor.Syntax.Keyword1.Style.Foreground);
            StyleSetBackground(wxSTC_C_WORD, prefs.TextEditor.Syntax.Keyword1.Style.Background);
            StyleSetBold      (wxSTC_C_WORD, prefs.TextEditor.Syntax.Keyword1.Style.Bold);
            StyleSetItalic    (wxSTC_C_WORD, prefs.TextEditor.Syntax.Keyword1.Style.Italic);

            // set keyword 2 style
            const wxArrayString& kw2 = prefs.TextEditor.Syntax.Keyword2.List;
            wxString kw2Str;
            for (size_t i = 0; i < kw2.GetCount(); i++) {
                kw2Str += kw2[i] + " ";
            }
            SetKeyWords(1, kw2Str);

            StyleSetForeground(wxSTC_C_WORD2, prefs.TextEditor.Syntax.Keyword2.Style.Foreground);
            StyleSetBackground(wxSTC_C_WORD2, prefs.TextEditor.Syntax.Keyword2.Style.Background);
            StyleSetBold      (wxSTC_C_WORD2, prefs.TextEditor.Syntax.Keyword2.Style.Bold);
            StyleSetItalic    (wxSTC_C_WORD2, prefs.TextEditor.Syntax.Keyword2.Style.Italic);
        }
    }

    if (wxFileName::FileExists(fileName)) {
        LoadFile(fileName);
        GotoPos(0);
    }

    if (prefs.TextEditor.Margin.FoldAllOnStartup) {
        foldAll();
    }

    // connect events
    Connect(wxEVT_STC_MARGINCLICK,      wxStyledTextEventHandler(TextEditor::onMarginClick));
    Connect(wxEVT_STC_UPDATEUI,         wxStyledTextEventHandler(TextEditor::onUpdateUI));
    Connect(wxEVT_STC_CHARADDED,        wxStyledTextEventHandler(TextEditor::onCharAdded));
    Connect(wxEVT_STC_SAVEPOINTREACHED, wxStyledTextEventHandler(TextEditor::onSavePointReached));
    Connect(wxEVT_STC_SAVEPOINTLEFT,    wxStyledTextEventHandler(TextEditor::onSavePointLeft));
}

//-------------------------------------------------------------------
TextEditor::~TextEditor()
{
}

//-------------------------------------------------------------------
void
TextEditor::foldAll()
{
    int maxLines = GetLineCount();
    for (int line = 0; line < maxLines; line++) {
        SetFoldExpanded(line, false);
        int lineMaxSubord = GetLastChild(line, -1);
        if (lineMaxSubord > line) {
            HideLines(line + 1, lineMaxSubord);
        }
    }
}

//-------------------------------------------------------------------
wxString
TextEditor::getEOLString() const
{
    switch (GetEOLMode()) {
    case wxSTC_EOL_CR:   return wxT("\r");
    case wxSTC_EOL_CRLF: return wxT("\r\n");
    case wxSTC_EOL_LF:   return wxT("\n");
    default:
        return wxT("\n");
    }
}

//-------------------------------------------------------------------
void
TextEditor::onMarginClick(wxStyledTextEvent& event)
{
    if (_scriptMode && event.GetMargin() == MARGIN_FOLD) {
        int lineClick = LineFromPosition(event.GetPosition());
        int levelClick = GetFoldLevel(lineClick);

        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0) {
            ToggleFold(lineClick);
        }
    }
}

//-------------------------------------------------------------------
void
TextEditor::onUpdateUI(wxStyledTextEvent& event)
{
    if (_scriptMode && _matchBraces) {
        // perform brace checking if a brace is nearby
        // first check the character to the right of the caret
        int l = GetCurrentPos();
        int c = GetCharAt(l);
        if (c == '{' || c == '}' ||
            c == '(' || c == ')' ||
            c == '[' || c == ']' ||
            c == '<' || c == '>')
        {
            int r = BraceMatch(l);
            if (r != wxSTC_INVALID_POSITION) {
                BraceHighlight(l, r);
            } else {
                BraceBadLight(l);
            }
            return;
        } else {
            // now check the character to the left of the caret
            l = GetCurrentPos() - 1;
            c = GetCharAt(l);
            if (c == '{' || c == '}' ||
                c == '(' || c == ')' ||
                c == '[' || c == ']' ||
                c == '<' || c == '>')
            {
                int r = BraceMatch(l);
                if (r != wxSTC_INVALID_POSITION) {
                    BraceHighlight(l, r);
                } else {
                    BraceBadLight(l);
                }
                return;
            }
        }
        // no braces nearby, cancel brace highlighting
        BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
    }
}

//-------------------------------------------------------------------
static bool is_valid_identifier_char(int c)
{
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        (c == '_'))
    {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
void
TextEditor::onCharAdded(wxStyledTextEvent& event)
{
    int currentPos  = GetCurrentPos();
    int currentLine = GetCurrentLine();
    int charAdded   = event.GetKey();

    if (_scriptMode) {
        if (_autoIndent && (charAdded == '\n' || charAdded == '\r')) {
            int lineIndent = 0;
            if (currentLine > 0) {
                lineIndent = GetLineIndentation(currentLine - 1);
                if (GetCharAt(GetLineEndPosition(currentLine - 1) - 1) == '{') {
                    lineIndent += GetIndent();
                }
            }
            SetLineIndentation(currentLine, lineIndent);
            GotoPos(GetLineEndPosition(currentLine));
/*
        } else if (_showCallTips && (charAdded == '(')) {
            int p = currentPos - 1;
            if (p > 0) {
                int s = 'S';
                int l = -1;
                int r = -1;
                while (!(s == 'C' || s == 'D')) {
                    int c = GetCharAt(p - 1);
                    switch (s) {
                    case 'S':
                        if (c == ' ') {
                            s = 'A';
                        } else if (is_valid_identifier_char(c)) {
                            s = 'B';
                            r = p;
                        } else {
                            s = 'D';
                        }
                        break;
                    case 'A':
                        if (is_valid_identifier_char(c)) {
                            s = 'B';
                            r = p;
                        } else if (c != ' ') {
                            s = 'D';
                        }
                        break;
                    case 'B':
                        if (!is_valid_identifier_char(c)) {
                            s = 'C';
                            l = p;
                        }
                        break;
                    }
                    p--;
                    if (p == 0) {
                        if (s == 'B') {
                            s = 'C';
                            l = p;
                        } else {
                            s = 'D';
                        }
                    } else {
                        c = GetCharAt(p - 1);
                    }
                }
                if (s == 'C') {
                    wxString funcName = GetTextRange(l, r);
                    if (funcName == "Foo") {
                        CallTipShow(currentPos - 4, wxT("Foo(a, b)"));
                        CallTipSetHighlight(4, 5);
                    }
                }
            }
        } else if (_showCallTips && (charAdded == ')')) {
            if (CallTipActive()) {
                CallTipCancel();
            }
*/
        } else if (_autoIndent && (charAdded == '}')) {
            if (currentPos - 1 == GetLineIndentPosition(currentLine)) {
                SetLineIndentation(currentLine, GetLineIndentation(currentLine) - GetIndent());
            }
        }
    }
}

//-------------------------------------------------------------------
void
TextEditor::onSavePointReached(wxStyledTextEvent& event)
{
    _developer->fileEditorSavePointReached(this);
}

//-------------------------------------------------------------------
void
TextEditor::onSavePointLeft(wxStyledTextEvent& event)
{
    _developer->fileEditorSavePointLeft(this);
}

//-------------------------------------------------------------------
const wxString&
TextEditor::getFileName() const
{
    return _fileName;
}

//-------------------------------------------------------------------
int
TextEditor::getFileEditorType() const
{
    return FET_TEXT;
}

//-------------------------------------------------------------------
bool
TextEditor::isFileModified() const
{
    return GetModify();
}

//-------------------------------------------------------------------
bool
TextEditor::saveFile()
{
    if (GetModify()) {
        if (wxFileName::FileExists(_fileName)) {
            if (SaveFile(_fileName)) {
                _developer->fileEditorFileSaved(this);
                return true;
            }
            return false;
        }

        return saveFileAs();
    }

    return false;
}

//-------------------------------------------------------------------
bool
TextEditor::saveFileAs(const wxString& fileName)
{
    wxString newFileName = fileName;

    if (newFileName == wxEmptyString) {
        wxString wildcardStr = (_scriptMode ? _developer->createScriptWildcardString() : _developer->createTextWildcardString());
        wxString defaultFile = wxFileName(_fileName).GetFullName();
        wxString defaultDir  = wxFileName(_fileName).GetPath();

        if (!wxDir::Exists(defaultDir)) {
            defaultDir = _developer->getPreferences().General.DataLocation;
        }

        // show save file as dialog
        wxFileDialog opd(this, wxT("Save File As"), defaultDir, defaultFile, wildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (opd.ShowModal() == wxID_CANCEL) {
            return false;
        }

        newFileName = opd.GetPath();
    }

    if (!SaveFile(newFileName)) {
        return false;
    }

    _fileName = newFileName;

    _developer->fileEditorFileSaved(this);

    return true;
}

//-------------------------------------------------------------------
bool
TextEditor::canDoCommand(int cmd) const
{
    switch (cmd) {
    case CMD_CUT:
    case CMD_COPY:
        return (GetSelectionStart() - GetSelectionEnd()) != 0;
    case CMD_PASTE:
        return CanPaste();
    case CMD_UNDO:
        return CanUndo();
    case CMD_REDO:
        return CanRedo();
    case CMD_TEXT_ZOOM_IN:
        return GetZoom() < 20;
    case CMD_TEXT_ZOOM_OUT:
        return GetZoom() > -10;
    case CMD_TEXT_INDENT:
    case CMD_TEXT_OUTDENT:
    case CMD_TEXT_COMMENT_OUT:
    case CMD_TEXT_UNCOMMENT:
        return (GetSelectionStart() - GetSelectionEnd()) != 0;
    case CMD_TEXT_MOVE_UP:
        return ((GetSelectionStart() - GetSelectionEnd()) != 0) && (LineFromPosition(GetSelectionStart()) > 0);
    case CMD_TEXT_MOVE_DOWN:
        return ((GetSelectionStart() - GetSelectionEnd()) != 0) && (LineFromPosition(GetSelectionEnd()) + 2 < GetLineCount()); // NOTE: "+ 2" because of a bug I wasn't able to solve yet
    case CMD_TEXT_GOTO:
        return GetLineCount() > 1;
    default:
        return false;
    }
}

//-------------------------------------------------------------------
void
TextEditor::doCommand(int cmd)
{
    switch (cmd) {
        case CMD_CUT:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
                Cut();
            }
            break;
        case CMD_COPY:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
                CopyRange(GetSelectionStart(), GetSelectionEnd());
            }
            break;
        case CMD_PASTE:
            if (CanPaste()) {
                Paste();
            }
            break;
        case CMD_UNDO:
            if (CanUndo()) {
                Undo();
            }
            break;
        case CMD_REDO:
            if (CanRedo()) {
                Redo();
            }
            break;
        case CMD_TEXT_ZOOM_IN:
            if (GetZoom() < 20) {
                int newZoomLevel = GetZoom() + 1;
                SetZoom(newZoomLevel);
                if (_displayLineNumbers) {
                    // adjust line numer margin so the line numbers will fit in
                    SetMarginWidth(MARGIN_LINENUMBER, MARGIN_LINENUMBER_WIDTH_BASE + (newZoomLevel > 0 ? newZoomLevel * MARGIN_LINENUMBER_WIDTH_MULTIPLIER : 0));
                }
            }
            break;
        case CMD_TEXT_ZOOM_OUT:
            if (GetZoom() > -10) {
                int newZoomLevel = GetZoom() - 1;
                SetZoom(newZoomLevel);
                if (_displayLineNumbers) {
                    // adjust line numer margin so the line numbers will fit in
                    SetMarginWidth(MARGIN_LINENUMBER, MARGIN_LINENUMBER_WIDTH_BASE + (newZoomLevel > 0 ? newZoomLevel * MARGIN_LINENUMBER_WIDTH_MULTIPLIER : 0));
                }
            }
            break;
        case CMD_TEXT_INDENT:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
                int indentSize = GetIndent();
                int firstLine  = LineFromPosition(GetSelectionStart());
                int lastLine   = LineFromPosition(GetSelectionEnd());
                if (firstLine > lastLine) {
                    std::swap(firstLine, lastLine);
                }
                for (int i = firstLine; i <= lastLine; i++) {
                    int lineIndentation = GetLineIndentation(i);
                    SetLineIndentation(i, lineIndentation + indentSize);
                }
                SetSelection(PositionFromLine(firstLine), GetLineEndPosition(lastLine));
            }
            break;
        case CMD_TEXT_OUTDENT:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
                int indentSize = GetIndent();
                int firstLine  = LineFromPosition(GetSelectionStart());
                int lastLine   = LineFromPosition(GetSelectionEnd());
                if (firstLine > lastLine) {
                    std::swap(firstLine, lastLine);
                }
                for (int i = firstLine; i <= lastLine; i++) {
                    int lineIndentation = GetLineIndentation(i);
                    SetLineIndentation(i, std::max(0, lineIndentation - indentSize));
                }
                SetSelection(PositionFromLine(firstLine), GetLineEndPosition(lastLine));
            }
            break;
        case CMD_TEXT_COMMENT_OUT:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
                int firstLine  = LineFromPosition(GetSelectionStart());
                int lastLine   = LineFromPosition(GetSelectionEnd());
                if (firstLine > lastLine) {
                    std::swap(firstLine, lastLine);
                }
                for (int i = firstLine; i <= lastLine; i++) {
                    int lineStart = PositionFromLine(i);
                    InsertText(lineStart, wxT("//"));
                }
                SetSelection(PositionFromLine(firstLine), GetLineEndPosition(lastLine));
            }
            break;
        case CMD_TEXT_UNCOMMENT:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
                int firstLine  = LineFromPosition(GetSelectionStart());
                int lastLine   = LineFromPosition(GetSelectionEnd());
                if (firstLine > lastLine) {
                    std::swap(firstLine, lastLine);
                }
                for (int i = firstLine; i <= lastLine; i++) {
                    if (LineLength(i) > 2) {
                        int lineStart = PositionFromLine(i);
                        wxString firstTwoChars = GetTextRange(lineStart, lineStart + 2);
                        if (firstTwoChars == wxT("//")) {
                            SetTargetStart(lineStart);
                            SetTargetEnd(lineStart + 2);
                            ReplaceTarget(wxT(""));
                        }
                    }
                }
                SetSelection(PositionFromLine(firstLine), GetLineEndPosition(lastLine));
            }
            break;
        case CMD_TEXT_MOVE_UP:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0 && LineFromPosition(GetSelectionStart()) > 0) {
                int firstLine = LineFromPosition(GetSelectionStart());
                int lastLine  = LineFromPosition(GetSelectionEnd());
                int upperLine = firstLine - 1;
                wxString upperLineText = GetLine(upperLine);
                GotoLine(upperLine);
                LineDelete();
                InsertText(PositionFromLine(lastLine), upperLineText);
                GotoLine(firstLine - 1);
                SetSelection(PositionFromLine(firstLine - 1), GetLineEndPosition(lastLine - 1));
            }
            break;
        case CMD_TEXT_MOVE_DOWN:
            if ((GetSelectionStart() - GetSelectionEnd()) != 0 && LineFromPosition(GetSelectionEnd()) + 2 < GetLineCount()) { // NOTE: "+ 2" because of a bug I wasn't able to solve yet
                int firstLine = LineFromPosition(GetSelectionStart());
                int lastLine  = LineFromPosition(GetSelectionEnd());
                int bottomLine = lastLine + 1;
                wxString bottomLineText = GetLine(bottomLine);
                GotoLine(bottomLine);
                LineDelete();
                InsertText(PositionFromLine(firstLine), bottomLineText);
                GotoLine(lastLine + 1);
                SetSelection(PositionFromLine(firstLine + 1), GetLineEndPosition(lastLine + 1));
            }
            break;
        case CMD_TEXT_GOTO:
            if (GetLineCount() > 1) {
                wxString maxLine;
                maxLine << GetLineCount();
                wxString promptStr = wxT("Line number (1 - ") + maxLine + wxT("):");
                wxTextEntryDialog gtld(this, promptStr, wxT("Go To Line"));
                if (gtld.ShowModal() == wxID_CANCEL) {
                    return;
                }
                wxString lineStr = gtld.GetValue();
                unsigned long line;
                if (!lineStr.ToULong(&line) || line == 0 || line > (unsigned long)GetLineCount()) {
                    wxMessageDialog errorDialog(this, wxEmptyString, wxT("Go To Line"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
                    errorDialog.SetMessage(wxT("Invalid line number."));
                    errorDialog.ShowModal();
                    return;
                }
                GotoLine(line - 1);
            }
            break;
    }
}
