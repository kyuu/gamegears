#include "ScriptEditor.hpp"
#include "Developer.hpp"

#include <iostream>
#include <fstream>
extern std::ofstream Log;

#define MARGIN_LINENUMBER 0
#define MARGIN_FOLD       1


//-------------------------------------------------------------------
ScriptEditor::ScriptEditor(Developer* developer, const wxString& filename)
    : wxStyledTextCtrl(developer)
    , _commandHandlerBridge(this)
    , _developer(developer)
    , _autoIndent(true)
    , _matchBraces(true)
    , _autoComplete(false)
    , _showCallTips(false)
{
    SetClientData(&_commandHandlerBridge);

    // connect events
    Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(ScriptEditor::onMarginClick), 0, this);
    Connect(wxEVT_STC_UPDATEUI,    wxStyledTextEventHandler(ScriptEditor::onUpdateUI),    0, this);
    Connect(wxEVT_STC_CHARADDED,   wxStyledTextEventHandler(ScriptEditor::onCharAdded),   0, this);

    // get preferences
    Preferences& prefs = developer->getPreferences();

    // set zoom level
    SetZoom(prefs.ScriptEditor.ZoomLevel);

    // clear all styles to defaults
    StyleClearAll();

    // setup indentation
    SetIndent(prefs.ScriptEditor.Indentation.IndentSize);
    SetUseTabs(false);
    SetBackSpaceUnIndents(prefs.ScriptEditor.Indentation.BackspaceUnindents);
    _autoIndent = prefs.ScriptEditor.Indentation.EnableAutoIndent;

    // setup line endings visibility
    SetViewEOL(prefs.ScriptEditor.ShowLineEndings);

    // setup whitespace visibility
    if (prefs.ScriptEditor.ShowWhitespace) {
        SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    } else {
        SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
    }

    // setup line number margin
    if (prefs.ScriptEditor.LineNumbers.Enabled) {
        SetMarginType(MARGIN_LINENUMBER, wxSTC_MARGIN_NUMBER);
        SetMarginWidth(MARGIN_LINENUMBER, prefs.ScriptEditor.LineNumbers.MarginWidth);
        SetMarginMask(MARGIN_LINENUMBER, ~wxSTC_MASK_FOLDERS);

        RgbColor& lnColor = prefs.ScriptEditor.LineNumbers.Color;
        StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(lnColor.Red, lnColor.Green, lnColor.Blue));
    }

    // setup folding
    if (prefs.ScriptEditor.Folding.Enabled) {
        SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
        SetMarginWidth(MARGIN_FOLD, prefs.ScriptEditor.Folding.MarginWidth);
        SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
        SetMarginSensitive(MARGIN_FOLD, true);

        SetProperty(wxT("fold"),         wxT("1"));
        SetProperty(wxT("fold.comment"), wxT("1"));
        SetProperty(wxT("fold.compact"), wxT("1"));

        wxColor grey( 100, 100, 100 );
        MarkerDefine (wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW );
        MarkerSetForeground (wxSTC_MARKNUM_FOLDER, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDER, grey);

        MarkerDefine (wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN);
        MarkerSetForeground (wxSTC_MARKNUM_FOLDEROPEN, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDEROPEN, grey);

        MarkerDefine (wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
        MarkerSetForeground (wxSTC_MARKNUM_FOLDERSUB, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDERSUB, grey);

        MarkerDefine (wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_ARROW);
        MarkerSetForeground (wxSTC_MARKNUM_FOLDEREND, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDEREND, _T("WHITE"));

        MarkerDefine (wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
        MarkerSetForeground (wxSTC_MARKNUM_FOLDEROPENMID, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDEROPENMID, _T("WHITE"));

        MarkerDefine (wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
        MarkerSetForeground (wxSTC_MARKNUM_FOLDERMIDTAIL, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDERMIDTAIL, grey);

        MarkerDefine (wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
        MarkerSetForeground (wxSTC_MARKNUM_FOLDERTAIL, grey);
        MarkerSetBackground (wxSTC_MARKNUM_FOLDERTAIL, grey);
    }

    // setup brace matching
    _matchBraces = prefs.ScriptEditor.BraceMatching.Enabled;

    RgbColor& hlColor = prefs.ScriptEditor.BraceMatching.HighlightColor;
    StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColour(hlColor.Red, hlColor.Green, hlColor.Blue));
    StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);

    RgbColor& blColor = prefs.ScriptEditor.BraceMatching.BadlightColor;
    StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(blColor.Red, blColor.Green, blColor.Blue));
    StyleSetBold(wxSTC_STYLE_BRACEBAD, true);

    // setup autocomplete
    _autoComplete = prefs.ScriptEditor.AutoComplete.Enabled;

    // setup calltips
    _autoComplete = prefs.ScriptEditor.CallTips.Enabled;

    // setup syntax highlighting
    SetLexer(wxSTC_LEX_CPP);

    RgbColor& stringColor = prefs.ScriptEditor.SyntaxHighlighting.String.Color;
    StyleSetForeground(wxSTC_C_STRING, wxColour(stringColor.Red, stringColor.Green, stringColor.Blue));
    StyleSetBold(wxSTC_C_STRING, prefs.ScriptEditor.SyntaxHighlighting.String.Bold);

    RgbColor& identifierColor = prefs.ScriptEditor.SyntaxHighlighting.Identifier.Color;
    StyleSetForeground(wxSTC_C_IDENTIFIER, wxColour(identifierColor.Red, identifierColor.Green, identifierColor.Blue));
    StyleSetBold(wxSTC_C_IDENTIFIER, prefs.ScriptEditor.SyntaxHighlighting.Identifier.Bold);

    RgbColor& numberColor = prefs.ScriptEditor.SyntaxHighlighting.Number.Color;
    StyleSetForeground(wxSTC_C_NUMBER, wxColour(numberColor.Red, numberColor.Green, numberColor.Blue));
    StyleSetBold(wxSTC_C_NUMBER, prefs.ScriptEditor.SyntaxHighlighting.Number.Bold);

    RgbColor& characterColor = prefs.ScriptEditor.SyntaxHighlighting.Character.Color;
    StyleSetForeground(wxSTC_C_CHARACTER, wxColour(characterColor.Red, characterColor.Green, characterColor.Blue));
    StyleSetBold(wxSTC_C_CHARACTER, prefs.ScriptEditor.SyntaxHighlighting.Character.Bold);

    RgbColor& commentColor = prefs.ScriptEditor.SyntaxHighlighting.Comment.Color;
    StyleSetForeground(wxSTC_C_PREPROCESSOR,           wxColour(commentColor.Red, commentColor.Green, commentColor.Blue));
    StyleSetForeground(wxSTC_C_COMMENT,                wxColour(commentColor.Red, commentColor.Green, commentColor.Blue));
    StyleSetForeground(wxSTC_C_COMMENTLINE,            wxColour(commentColor.Red, commentColor.Green, commentColor.Blue));
    StyleSetForeground(wxSTC_C_COMMENTDOC,             wxColour(commentColor.Red, commentColor.Green, commentColor.Blue));
    StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD,      wxColour(commentColor.Red, commentColor.Green, commentColor.Blue));
    StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(commentColor.Red, commentColor.Green, commentColor.Blue));
    StyleSetBold(wxSTC_C_PREPROCESSOR,           prefs.ScriptEditor.SyntaxHighlighting.Comment.Bold);
    StyleSetBold(wxSTC_C_COMMENT,                prefs.ScriptEditor.SyntaxHighlighting.Comment.Bold);
    StyleSetBold(wxSTC_C_COMMENTLINE,            prefs.ScriptEditor.SyntaxHighlighting.Comment.Bold);
    StyleSetBold(wxSTC_C_COMMENTDOC,             prefs.ScriptEditor.SyntaxHighlighting.Comment.Bold);
    StyleSetBold(wxSTC_C_COMMENTDOCKEYWORD,      prefs.ScriptEditor.SyntaxHighlighting.Comment.Bold);
    StyleSetBold(wxSTC_C_COMMENTDOCKEYWORDERROR, prefs.ScriptEditor.SyntaxHighlighting.Comment.Bold);

    // setup primary keywords
    std::vector<std::string>& pkw = prefs.ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List;
    std::string pkwString;
    for (size_t i = 0; i < pkw.size(); i++) {
        pkwString += pkw[i] + " ";
    }
    SetKeyWords(0, pkwString.c_str());

    RgbColor& pkwColor = prefs.ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color;
    StyleSetForeground(wxSTC_C_WORD, wxColour(pkwColor.Red, pkwColor.Green, pkwColor.Blue));
    StyleSetBold(wxSTC_C_WORD, prefs.ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Bold);

    // setup secondary keywords
    std::vector<std::string>& skw = prefs.ScriptEditor.SyntaxHighlighting.SecondaryKeyword.List;
    std::string skwString;
    for (size_t i = 0; i < skw.size(); i++) {
        skwString += skw[i] + " ";
    }
    SetKeyWords(1, skwString.c_str());

    RgbColor& skwColor = prefs.ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color;
    StyleSetForeground(wxSTC_C_WORD2, wxColour(skwColor.Red, skwColor.Green, skwColor.Blue));
    StyleSetBold(wxSTC_C_WORD2, prefs.ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Bold);

    if (!filename.IsEmpty()) {
        LoadFile(filename);
        GotoPos(0);
    }
}

//-------------------------------------------------------------------
ScriptEditor::~ScriptEditor()
{
}

//-------------------------------------------------------------------
void
ScriptEditor::onMarginClick(wxStyledTextEvent& e)
{
    if (e.GetMargin() == MARGIN_FOLD) {
        int lineClick = LineFromPosition(e.GetPosition());
        int levelClick = GetFoldLevel(lineClick);

        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0) {
            ToggleFold(lineClick);
        }
    }
}

//-------------------------------------------------------------------
void
ScriptEditor::onUpdateUI(wxStyledTextEvent& e)
{
    if (_matchBraces) {
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
ScriptEditor::onCharAdded(wxStyledTextEvent& e)
{
    int currentPos  = GetCurrentPos();
    int currentLine = GetCurrentLine();
    int charAdded   = e.GetKey();

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
    } else if (_autoIndent && (charAdded == '}')) {
        if (currentPos - 1 == GetLineIndentPosition(currentLine)) {
            SetLineIndentation(currentLine, GetLineIndentation(currentLine) - GetIndent());
        }
    }
}

//-------------------------------------------------------------------
wxString
ScriptEditor::getEOLString() const
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
bool
ScriptEditor::canDoCommand(int cmd) const
{
    switch (cmd) {
    case ID_CUT:
    case ID_COPY:
        return (GetSelectionStart() - GetSelectionEnd()) != 0;
    case ID_PASTE:
        return CanPaste();
    case ID_UNDO:
        return CanUndo();
    case ID_REDO:
        return CanRedo();
    case ID_ZOOM_IN:
        return GetZoom() < 20;
    case ID_ZOOM_OUT:
        return GetZoom() > -5;
    default:
        return false;
    }
}

//-------------------------------------------------------------------
void
ScriptEditor::handleCommand(int cmd)
{
    switch (cmd) {
    case ID_CUT:
        if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
            Cut();
        }
        break;
    case ID_COPY:
        if ((GetSelectionStart() - GetSelectionEnd()) != 0) {
            CopyRange(GetSelectionStart(), GetSelectionEnd());
        }
        break;
    case ID_PASTE:
        if (CanPaste()) {
            Paste();
        }
        break;
    case ID_UNDO:
        if (CanUndo()) {
            Undo();
        }
        break;
    case ID_REDO:
        if (CanRedo()) {
            Redo();
        }
        break;
    case ID_ZOOM_IN:
        if (GetZoom() < 20) {
            int newZoomLevel = GetZoom() + 1;
            SetZoom(newZoomLevel);
            _developer->getPreferences().ScriptEditor.ZoomLevel = newZoomLevel;
        }
        break;
    case ID_ZOOM_OUT:
        if (GetZoom() > -5) {
            int newZoomLevel = GetZoom() - 1;
            SetZoom(newZoomLevel);
            _developer->getPreferences().ScriptEditor.ZoomLevel = newZoomLevel;
        }
        break;
    }
}
