#ifndef SCRIPTEDITOR_HPP
#define SCRIPTEDITOR_HPP

#include <vector>
#include <string>
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include "CommandHandlerBridge.hpp"


// forward declaration
class Developer;


class ScriptEditor : public wxStyledTextCtrl {
public:
    enum {
    };

    ScriptEditor(Developer* developer, const wxString& filename);
    ~ScriptEditor();

    // event handlers
    void onMarginClick(wxStyledTextEvent& e);
    void onUpdateUI(wxStyledTextEvent& e);
    void onCharAdded(wxStyledTextEvent& e);

    // used by the command handler bridge
    bool canDoCommand(int cmd) const;
    void handleCommand(int cmd);

private:
    // auxiliary method
    wxString getEOLString() const;

private:
    CommandHandlerBridge<ScriptEditor> _commandHandlerBridge;
    Developer* _developer;

    bool _autoIndent;
    bool _matchBraces;
    bool _autoComplete;
    bool _showCallTips;

    wxString _filename;

};


#endif
