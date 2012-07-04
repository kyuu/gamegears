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

#include <fstream>
#include <wx/wx.h>
#include "json/json.h"
#include "Preferences.hpp"

#define DEFAULT_DEVELOPERWINDOW_WIDTH  800
#define DEFAULT_DEVELOPERWINDOW_HEIGHT 600
#define DEFAULT_DEVELOPERWINDOW_POSX   wxDefaultCoord
#define DEFAULT_DEVELOPERWINDOW_POSY   wxDefaultCoord


//-------------------------------------------------------------------
static wxString DefaultPrimaryKeywords[] = {
    "if",
    "else",
    "switch",
    "case",
    "default",
    "while",
    "do",
    "for",
    "foreach",
    "in",
    "break",
    "continue",
    "return",
    "yield",
    "resume",
    "clone",
    "delete",
    "typeof",
    "instanceof",
    "static",
    "const",
    "enum",
    "local",
    "function",
    "class",
    "extends",
    "try",
    "catch",
    "throw",
    "this",
    "base",
    "constructor",
    "true",
    "false",
    "null",
    "", // end of keywords
};

static wxString DefaultSecondaryKeywords[] = {
    "", // end of keywords
};

static wxString DefaultProjectFileTypes[] = {
    "ggproj",
    "", // end of file types
};

static wxString DefaultScriptFileTypes[] = {
    "ggs",
    "nut",
    "sq",
    "", // end of file types
};

static wxString DefaultTextFileTypes[] = {
    "ini",
    "txt",
    "", // end of file types
};


//-------------------------------------------------------------------
static wxColour get_color(Json::Value& val, const wxColour& defCol)
{
    int r = val.get("Red",   defCol.Red()).asInt();
    int g = val.get("Green", defCol.Green()).asInt();
    int b = val.get("Blue",  defCol.Blue()).asInt();

    return wxColour(r, g, b);
}

//-------------------------------------------------------------------
Preferences::Preferences()
{
    loadDefaults();
}

//-------------------------------------------------------------------
Preferences::Preferences(const Preferences& that)
{
    *this = that;
}

//-------------------------------------------------------------------
Preferences::~Preferences()
{
}

//-------------------------------------------------------------------
Preferences&
Preferences::operator=(const Preferences& that)
{
    General.Window.Width       = that.General.Window.Width;
    General.Window.Height      = that.General.Window.Height;
    General.Window.X           = that.General.Window.X;
    General.Window.Y           = that.General.Window.Y;
    General.Window.Maximized   = that.General.Window.Maximized;

    General.FileTypeAssociation.Project = that.General.FileTypeAssociation.Project;
    General.FileTypeAssociation.Script  = that.General.FileTypeAssociation.Script;
    General.FileTypeAssociation.Text    = that.General.FileTypeAssociation.Text;

    General.DataLocation       = that.General.DataLocation;
    General.CommonDataLocation = that.General.CommonDataLocation;

    General.OpenLastProjectOnStartup = that.General.OpenLastProjectOnStartup;

    General.RecentFiles    = that.General.RecentFiles;
    General.RecentProjects = that.General.RecentProjects;

    General.DockManagerState = that.General.DockManagerState;

    TextEditor.General.DefaultStyle.Foreground = that.TextEditor.General.DefaultStyle.Foreground;
    TextEditor.General.DefaultStyle.Background = that.TextEditor.General.DefaultStyle.Background;

    TextEditor.General.Caret.Width = that.TextEditor.General.Caret.Width;
    TextEditor.General.Caret.Color = that.TextEditor.General.Caret.Color;

    TextEditor.General.Font.FaceName  = that.TextEditor.General.Font.FaceName;
    TextEditor.General.Font.PointSize = that.TextEditor.General.Font.PointSize;

    TextEditor.General.InitialZoomLevel = that.TextEditor.General.InitialZoomLevel;

    TextEditor.General.EnableAutoComplete = that.TextEditor.General.EnableAutoComplete;
    TextEditor.General.EnableCallTips     = that.TextEditor.General.EnableCallTips;

    TextEditor.Margin.DisplayLineNumbers         = that.TextEditor.Margin.DisplayLineNumbers;
    TextEditor.Margin.LineNumberStyle.Bold       = that.TextEditor.Margin.LineNumberStyle.Bold;
    TextEditor.Margin.LineNumberStyle.Italic     = that.TextEditor.Margin.LineNumberStyle.Italic;
    TextEditor.Margin.LineNumberStyle.Foreground = that.TextEditor.Margin.LineNumberStyle.Foreground;
    TextEditor.Margin.LineNumberStyle.Background = that.TextEditor.Margin.LineNumberStyle.Background;

    TextEditor.Margin.DisplayFolders                  = that.TextEditor.Margin.DisplayFolders;
    TextEditor.Margin.FoldAllOnStartup                = that.TextEditor.Margin.FoldAllOnStartup;
    TextEditor.Margin.FolderMarginStyle.Foreground    = that.TextEditor.Margin.FolderMarginStyle.Foreground;
    TextEditor.Margin.FolderMarginStyle.Background    = that.TextEditor.Margin.FolderMarginStyle.Background;
    TextEditor.Margin.FolderHeaderStyle.Shape         = that.TextEditor.Margin.FolderHeaderStyle.Shape;
    TextEditor.Margin.FolderHeaderStyle.Foreground    = that.TextEditor.Margin.FolderHeaderStyle.Foreground;
    TextEditor.Margin.FolderHeaderStyle.Background    = that.TextEditor.Margin.FolderHeaderStyle.Background;
    TextEditor.Margin.FolderMidHeaderStyle.Shape      = that.TextEditor.Margin.FolderMidHeaderStyle.Shape;
    TextEditor.Margin.FolderMidHeaderStyle.Foreground = that.TextEditor.Margin.FolderMidHeaderStyle.Foreground;
    TextEditor.Margin.FolderMidHeaderStyle.Background = that.TextEditor.Margin.FolderMidHeaderStyle.Background;
    TextEditor.Margin.FolderBodyStyle.Shape           = that.TextEditor.Margin.FolderBodyStyle.Shape;
    TextEditor.Margin.FolderBodyStyle.Foreground      = that.TextEditor.Margin.FolderBodyStyle.Foreground;
    TextEditor.Margin.FolderBodyStyle.Background      = that.TextEditor.Margin.FolderBodyStyle.Background;

    TextEditor.Indent.TabSize               = that.TextEditor.Indent.TabSize;
    TextEditor.Indent.ConvertTabsToSpaces   = that.TextEditor.Indent.ConvertTabsToSpaces;
    TextEditor.Indent.IndentSize            = that.TextEditor.Indent.IndentSize;
    TextEditor.Indent.EnableAutoIndent      = that.TextEditor.Indent.EnableAutoIndent;
    TextEditor.Indent.BackspaceUnindents    = that.TextEditor.Indent.BackspaceUnindents;
    TextEditor.Indent.ShowIndentationGuides = that.TextEditor.Indent.ShowIndentationGuides;
    TextEditor.Indent.IndentationGuideStyle.Foreground = that.TextEditor.Indent.IndentationGuideStyle.Foreground;
    TextEditor.Indent.IndentationGuideStyle.Background = that.TextEditor.Indent.IndentationGuideStyle.Background;

    TextEditor.Braces.EnableBraceMatching       = that.TextEditor.Braces.EnableBraceMatching;
    TextEditor.Braces.HighlightStyle.Bold       = that.TextEditor.Braces.HighlightStyle.Bold;
    TextEditor.Braces.HighlightStyle.Italic     = that.TextEditor.Braces.HighlightStyle.Italic;
    TextEditor.Braces.HighlightStyle.Foreground = that.TextEditor.Braces.HighlightStyle.Foreground;
    TextEditor.Braces.HighlightStyle.Background = that.TextEditor.Braces.HighlightStyle.Background;
    TextEditor.Braces.BadlightStyle.Bold        = that.TextEditor.Braces.BadlightStyle.Bold;
    TextEditor.Braces.BadlightStyle.Italic      = that.TextEditor.Braces.BadlightStyle.Italic;
    TextEditor.Braces.BadlightStyle.Foreground  = that.TextEditor.Braces.BadlightStyle.Foreground;
    TextEditor.Braces.BadlightStyle.Background  = that.TextEditor.Braces.BadlightStyle.Background;

    TextEditor.Syntax.EnableSyntaxHighlighting    = that.TextEditor.Syntax.EnableSyntaxHighlighting;

    TextEditor.Syntax.Operator.Style.Bold         = that.TextEditor.Syntax.Operator.Style.Bold;
    TextEditor.Syntax.Operator.Style.Italic       = that.TextEditor.Syntax.Operator.Style.Italic;
    TextEditor.Syntax.Operator.Style.Foreground   = that.TextEditor.Syntax.Operator.Style.Foreground;
    TextEditor.Syntax.Operator.Style.Background   = that.TextEditor.Syntax.Operator.Style.Background;

    TextEditor.Syntax.String.Style.Bold           = that.TextEditor.Syntax.String.Style.Bold;
    TextEditor.Syntax.String.Style.Italic         = that.TextEditor.Syntax.String.Style.Italic;
    TextEditor.Syntax.String.Style.Foreground     = that.TextEditor.Syntax.String.Style.Foreground;
    TextEditor.Syntax.String.Style.Background     = that.TextEditor.Syntax.String.Style.Background;

    TextEditor.Syntax.StringEOL.Style.Bold           = that.TextEditor.Syntax.StringEOL.Style.Bold;
    TextEditor.Syntax.StringEOL.Style.Italic         = that.TextEditor.Syntax.StringEOL.Style.Italic;
    TextEditor.Syntax.StringEOL.Style.Foreground     = that.TextEditor.Syntax.StringEOL.Style.Foreground;
    TextEditor.Syntax.StringEOL.Style.Background     = that.TextEditor.Syntax.StringEOL.Style.Background;

    TextEditor.Syntax.Identifier.Style.Bold       = that.TextEditor.Syntax.Identifier.Style.Bold;
    TextEditor.Syntax.Identifier.Style.Italic     = that.TextEditor.Syntax.Identifier.Style.Italic;
    TextEditor.Syntax.Identifier.Style.Foreground = that.TextEditor.Syntax.Identifier.Style.Foreground;
    TextEditor.Syntax.Identifier.Style.Background = that.TextEditor.Syntax.Identifier.Style.Background;

    TextEditor.Syntax.Number.Style.Bold           = that.TextEditor.Syntax.Number.Style.Bold;
    TextEditor.Syntax.Number.Style.Italic         = that.TextEditor.Syntax.Number.Style.Italic;
    TextEditor.Syntax.Number.Style.Foreground     = that.TextEditor.Syntax.Number.Style.Foreground;
    TextEditor.Syntax.Number.Style.Background     = that.TextEditor.Syntax.Number.Style.Background;

    TextEditor.Syntax.Character.Style.Bold        = that.TextEditor.Syntax.Character.Style.Bold;
    TextEditor.Syntax.Character.Style.Italic      = that.TextEditor.Syntax.Character.Style.Italic;
    TextEditor.Syntax.Character.Style.Foreground  = that.TextEditor.Syntax.Character.Style.Foreground;
    TextEditor.Syntax.Character.Style.Background  = that.TextEditor.Syntax.Character.Style.Background;

    TextEditor.Syntax.Comment.Style.Bold          = that.TextEditor.Syntax.Comment.Style.Bold;
    TextEditor.Syntax.Comment.Style.Italic        = that.TextEditor.Syntax.Comment.Style.Italic;
    TextEditor.Syntax.Comment.Style.Foreground    = that.TextEditor.Syntax.Comment.Style.Foreground;
    TextEditor.Syntax.Comment.Style.Background    = that.TextEditor.Syntax.Comment.Style.Background;

    TextEditor.Syntax.Keyword1.Style.Bold         = that.TextEditor.Syntax.Keyword1.Style.Bold;
    TextEditor.Syntax.Keyword1.Style.Italic       = that.TextEditor.Syntax.Keyword1.Style.Italic;
    TextEditor.Syntax.Keyword1.Style.Foreground   = that.TextEditor.Syntax.Keyword1.Style.Foreground;
    TextEditor.Syntax.Keyword1.Style.Background   = that.TextEditor.Syntax.Keyword1.Style.Background;
    TextEditor.Syntax.Keyword1.List               = that.TextEditor.Syntax.Keyword1.List;

    TextEditor.Syntax.Keyword2.Style.Bold         = that.TextEditor.Syntax.Keyword2.Style.Bold;
    TextEditor.Syntax.Keyword2.Style.Italic       = that.TextEditor.Syntax.Keyword2.Style.Italic;
    TextEditor.Syntax.Keyword2.Style.Foreground   = that.TextEditor.Syntax.Keyword2.Style.Foreground;
    TextEditor.Syntax.Keyword2.Style.Background   = that.TextEditor.Syntax.Keyword2.Style.Background;
    TextEditor.Syntax.Keyword2.List               = that.TextEditor.Syntax.Keyword2.List;

    return *this;
}

//-------------------------------------------------------------------
void
Preferences::loadDefaults()
{
    // load system defaults
    General.Window.Width       = 800;
    General.Window.Height      = 600;
    General.Window.X           = wxDefaultCoord;
    General.Window.Y           = wxDefaultCoord;
    General.Window.Maximized   = false;

    General.FileTypeAssociation.Project.Clear();
    for (int i = 0; !DefaultProjectFileTypes[i].IsEmpty(); i++) {
        General.FileTypeAssociation.Project.Add(DefaultProjectFileTypes[i]);
    }

    General.FileTypeAssociation.Script.Clear();
    for (int i = 0; !DefaultScriptFileTypes[i].IsEmpty(); i++) {
        General.FileTypeAssociation.Script.Add(DefaultScriptFileTypes[i]);
    }

    General.FileTypeAssociation.Text.Clear();
    for (int i = 0; !DefaultTextFileTypes[i].IsEmpty(); i++) {
        General.FileTypeAssociation.Text.Add(DefaultTextFileTypes[i]);
    }

    General.DataLocation       = "data";
    General.CommonDataLocation = "common";

    General.OpenLastProjectOnStartup = true;

    General.RecentFiles.Clear();
    General.RecentProjects.Clear();

    General.DockManagerState = "";

    // load text editor defaults
    TextEditor.General.DefaultStyle.Foreground = wxColour(  0,   0,   0);
    TextEditor.General.DefaultStyle.Background = wxColour(255, 255, 255);

    TextEditor.General.Caret.Width = 1;
    TextEditor.General.Caret.Color = wxColour(0, 0, 0);

    TextEditor.General.Font.FaceName  = "DejaVu Sans Mono";
    TextEditor.General.Font.PointSize = 10;

    TextEditor.General.InitialZoomLevel = 0;

    TextEditor.General.EnableAutoComplete = false;
    TextEditor.General.EnableCallTips     = false;

    TextEditor.Margin.DisplayLineNumbers         = true;
    TextEditor.Margin.LineNumberStyle.Bold       = false;
    TextEditor.Margin.LineNumberStyle.Italic     = false;
    TextEditor.Margin.LineNumberStyle.Foreground = wxColour( 75,  75,  75);
    TextEditor.Margin.LineNumberStyle.Background = wxColour(220, 220, 220);

    TextEditor.Margin.DisplayFolders                  = true;
    TextEditor.Margin.FoldAllOnStartup                = false;
    TextEditor.Margin.FolderMarginStyle.Foreground    = wxColour(175, 175, 175);
    TextEditor.Margin.FolderMarginStyle.Background    = wxColour(255, 255, 255);
    TextEditor.Margin.FolderHeaderStyle.Shape         = TE_FOLDER_HEADER_STYLE_ARROW;
    TextEditor.Margin.FolderHeaderStyle.Foreground    = wxColour(100, 100, 100);
    TextEditor.Margin.FolderHeaderStyle.Background    = wxColour(100, 100, 100);
    TextEditor.Margin.FolderMidHeaderStyle.Shape      = TE_FOLDER_HEADER_STYLE_PLUS_MINUS;
    TextEditor.Margin.FolderMidHeaderStyle.Foreground = wxColour(100, 100, 100);
    TextEditor.Margin.FolderMidHeaderStyle.Background = wxColour(255, 255, 255);
    TextEditor.Margin.FolderBodyStyle.Shape           = TE_FOLDER_BODY_STYLE_CURVED_LINE;
    TextEditor.Margin.FolderBodyStyle.Foreground      = wxColour(100, 100, 100);
    TextEditor.Margin.FolderBodyStyle.Background      = wxColour(100, 100, 100);

    TextEditor.Indent.TabSize               = 4;
    TextEditor.Indent.ConvertTabsToSpaces   = true;
    TextEditor.Indent.IndentSize            = 4;
    TextEditor.Indent.EnableAutoIndent      = true;
    TextEditor.Indent.BackspaceUnindents    = true;
    TextEditor.Indent.ShowIndentationGuides = false;
    TextEditor.Indent.IndentationGuideStyle.Foreground = wxColour(  0,   0,   0);
    TextEditor.Indent.IndentationGuideStyle.Background = wxColour(255, 255, 255);

    TextEditor.Braces.EnableBraceMatching       = true;
    TextEditor.Braces.HighlightStyle.Bold       = true;
    TextEditor.Braces.HighlightStyle.Italic     = false;
    TextEditor.Braces.HighlightStyle.Foreground = wxColour(  0, 128,   0);
    TextEditor.Braces.HighlightStyle.Background = wxColour(255, 255, 255);
    TextEditor.Braces.BadlightStyle.Bold        = true;
    TextEditor.Braces.BadlightStyle.Italic      = false;
    TextEditor.Braces.BadlightStyle.Foreground  = wxColour(255,   0,   0);
    TextEditor.Braces.BadlightStyle.Background  = wxColour(255, 255, 255);

    TextEditor.Syntax.EnableSyntaxHighlighting = true;

    TextEditor.Syntax.Operator.Style.Bold       = false;
    TextEditor.Syntax.Operator.Style.Italic     = false;
    TextEditor.Syntax.Operator.Style.Foreground = wxColour(200,   0, 200);
    TextEditor.Syntax.Operator.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.String.Style.Bold       = false;
    TextEditor.Syntax.String.Style.Italic     = false;
    TextEditor.Syntax.String.Style.Foreground = wxColour(  0, 128, 128);
    TextEditor.Syntax.String.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.StringEOL.Style.Bold       = false;
    TextEditor.Syntax.StringEOL.Style.Italic     = false;
    TextEditor.Syntax.StringEOL.Style.Foreground = wxColour(255,   0,   0);
    TextEditor.Syntax.StringEOL.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Identifier.Style.Bold       = false;
    TextEditor.Syntax.Identifier.Style.Italic     = false;
    TextEditor.Syntax.Identifier.Style.Foreground = wxColour(  0,   0,   0);
    TextEditor.Syntax.Identifier.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Number.Style.Bold       = false;
    TextEditor.Syntax.Number.Style.Italic     = false;
    TextEditor.Syntax.Number.Style.Foreground = wxColour(128,   0,   0);
    TextEditor.Syntax.Number.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Character.Style.Bold       = false;
    TextEditor.Syntax.Character.Style.Italic     = false;
    TextEditor.Syntax.Character.Style.Foreground = wxColour(128, 128,   0);
    TextEditor.Syntax.Character.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Comment.Style.Bold       = false;
    TextEditor.Syntax.Comment.Style.Italic     = false;
    TextEditor.Syntax.Comment.Style.Foreground = wxColour(  0, 128,   0);
    TextEditor.Syntax.Comment.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Keyword1.Style.Bold       = false;
    TextEditor.Syntax.Keyword1.Style.Italic     = false;
    TextEditor.Syntax.Keyword1.Style.Foreground = wxColour(  0,   0, 255);
    TextEditor.Syntax.Keyword1.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Keyword1.List.Clear();
    for (int i = 0; !DefaultPrimaryKeywords[i].IsEmpty(); i++) {
        TextEditor.Syntax.Keyword1.List.Add(DefaultPrimaryKeywords[i]);
    }

    TextEditor.Syntax.Keyword2.Style.Bold       = false;
    TextEditor.Syntax.Keyword2.Style.Italic     = false;
    TextEditor.Syntax.Keyword2.Style.Foreground = wxColour(  0,   0, 128);
    TextEditor.Syntax.Keyword2.Style.Background = wxColour(255, 255, 255);

    TextEditor.Syntax.Keyword2.List.Clear();
    for (int i = 0; !DefaultSecondaryKeywords[i].IsEmpty(); i++) {
        TextEditor.Syntax.Keyword2.List.Add(DefaultSecondaryKeywords[i]);
    }
}

//-------------------------------------------------------------------
bool
Preferences::loadFromFile(const std::string& fileName)
{
    // open configuration file
    std::ifstream prefs_file(fileName.c_str());
    if (!prefs_file.is_open()) {
        return false;
    }

    // parse configuration
    Json::Value prefs;
    Json::Reader reader;
    if (!reader.parse(prefs_file, prefs)) {
        return false;
    }

    Json::Value val; // temporary variable

    // load general preferences
    General.Window.Width       = prefs["General"]["Window"].get("Width",       General.Window.Width      ).asInt();
    General.Window.Height      = prefs["General"]["Window"].get("Height",      General.Window.Height     ).asInt();
    General.Window.X           = prefs["General"]["Window"].get("X",           General.Window.X          ).asInt();
    General.Window.Y           = prefs["General"]["Window"].get("Y",           General.Window.Y          ).asInt();
    General.Window.Maximized   = prefs["General"]["Window"].get("Maximized",   General.Window.Maximized  ).asBool();

    General.FileTypeAssociation.Project.Clear();
    val = prefs["General"]["FileTypeAssociation"]["Project"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                General.FileTypeAssociation.Project.Add(val[i].asString());
            }
        }
    }

    General.FileTypeAssociation.Script.Clear();
    val = prefs["General"]["FileTypeAssociation"]["Script"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                General.FileTypeAssociation.Script.Add(val[i].asString());
            }
        }
    }

    General.FileTypeAssociation.Text.Clear();
    val = prefs["General"]["FileTypeAssociation"]["Text"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                General.FileTypeAssociation.Text.Add(val[i].asString());
            }
        }
    }

    General.DataLocation       = prefs["General"].get("DataLocation",       General.DataLocation.ToStdString()).asString();
    General.CommonDataLocation = prefs["General"].get("CommonDataLocation", General.CommonDataLocation.ToStdString()).asString();

    General.OpenLastProjectOnStartup = prefs["General"].get("OpenLastProjectOnStartup", General.OpenLastProjectOnStartup).asBool();

    General.RecentFiles.Clear();
    val = prefs["General"]["RecentFiles"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < 10 && i < val.size(); i++) {
            if (val[i].isString()) {
                General.RecentFiles.Add(val[i].asString());
            }
        }
    }

    General.RecentProjects.Clear();
    val = prefs["General"]["RecentProjects"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < 10 &&  i < val.size(); i++) {
            if (val[i].isString()) {
                General.RecentProjects.Add(val[i].asString());
            }
        }
    }

    General.DockManagerState = prefs["General"].get("DockManagerState", General.DockManagerState.ToStdString()).asString();

    // load text editor preferences
    TextEditor.General.DefaultStyle.Foreground = get_color(prefs["TextEditor"]["General"]["DefaultStyle"]["Foreground"], TextEditor.General.DefaultStyle.Foreground);
    TextEditor.General.DefaultStyle.Background = get_color(prefs["TextEditor"]["General"]["DefaultStyle"]["Background"], TextEditor.General.DefaultStyle.Background);

    TextEditor.General.Caret.Width =           prefs["TextEditor"]["General"]["Caret"].get("Width", TextEditor.General.Caret.Width).asInt();
    TextEditor.General.Caret.Color = get_color(prefs["TextEditor"]["General"]["Caret"]["Color"],    TextEditor.General.Caret.Color);

    TextEditor.General.Font.FaceName  = prefs["TextEditor"]["General"]["Font"].get("FaceName",  TextEditor.General.Font.FaceName.ToStdString()).asString();
    TextEditor.General.Font.PointSize = prefs["TextEditor"]["General"]["Font"].get("PointSize", TextEditor.General.Font.PointSize             ).asInt();

    TextEditor.General.InitialZoomLevel = prefs["TextEditor"]["General"].get("InitialZoomLevel", TextEditor.General.InitialZoomLevel).asInt();

    TextEditor.General.EnableAutoComplete = prefs["TextEditor"]["General"].get("EnableAutoComplete", TextEditor.General.EnableAutoComplete).asBool();
    TextEditor.General.EnableCallTips     = prefs["TextEditor"]["General"].get("EnableCallTips",     TextEditor.General.EnableCallTips    ).asBool();

    TextEditor.Margin.DisplayLineNumbers          =           prefs["TextEditor"]["Margin"].get("DisplayLineNumbers",         TextEditor.Margin.DisplayLineNumbers).asBool();
    TextEditor.Margin.LineNumberStyle.Bold        =           prefs["TextEditor"]["Margin"]["LineNumberStyle"].get("Bold",    TextEditor.Margin.LineNumberStyle.Bold  ).asBool();
    TextEditor.Margin.LineNumberStyle.Italic      =           prefs["TextEditor"]["Margin"]["LineNumberStyle"].get("Italic",  TextEditor.Margin.LineNumberStyle.Italic).asBool();
    TextEditor.Margin.LineNumberStyle.Foreground  = get_color(prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Foreground"], TextEditor.Margin.LineNumberStyle.Foreground);
    TextEditor.Margin.LineNumberStyle.Background  = get_color(prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Background"], TextEditor.Margin.LineNumberStyle.Background);

    TextEditor.Margin.DisplayFolders                   =           prefs["TextEditor"]["Margin"].get("DisplayFolders",                  TextEditor.Margin.DisplayFolders).asBool();
    TextEditor.Margin.FoldAllOnStartup                 =           prefs["TextEditor"]["Margin"].get("FoldAllOnStartup",                TextEditor.Margin.FoldAllOnStartup).asBool();
    TextEditor.Margin.FolderMarginStyle.Foreground     = get_color(prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Foreground"],    TextEditor.Margin.FolderMarginStyle.Foreground);
    TextEditor.Margin.FolderMarginStyle.Background     = get_color(prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Background"],    TextEditor.Margin.FolderMarginStyle.Background);
    TextEditor.Margin.FolderHeaderStyle.Shape          =           prefs["TextEditor"]["Margin"]["FolderHeaderStyle"].get("Shape",      TextEditor.Margin.FolderHeaderStyle.Shape).asInt();
    TextEditor.Margin.FolderHeaderStyle.Foreground     = get_color(prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Foreground"],    TextEditor.Margin.FolderHeaderStyle.Foreground);
    TextEditor.Margin.FolderHeaderStyle.Background     = get_color(prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Background"],    TextEditor.Margin.FolderHeaderStyle.Background);
    TextEditor.Margin.FolderMidHeaderStyle.Shape            =      prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"].get("Shape",   TextEditor.Margin.FolderMidHeaderStyle.Shape).asInt();
    TextEditor.Margin.FolderMidHeaderStyle.Foreground  = get_color(prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Foreground"], TextEditor.Margin.FolderMidHeaderStyle.Foreground);
    TextEditor.Margin.FolderMidHeaderStyle.Background  = get_color(prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Background"], TextEditor.Margin.FolderMidHeaderStyle.Background);
    TextEditor.Margin.FolderBodyStyle.Shape                 =      prefs["TextEditor"]["Margin"]["FolderBodyStyle"].get("Shape",        TextEditor.Margin.FolderBodyStyle.Shape).asInt();
    TextEditor.Margin.FolderBodyStyle.Foreground       = get_color(prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Foreground"],      TextEditor.Margin.FolderBodyStyle.Foreground);
    TextEditor.Margin.FolderBodyStyle.Background       = get_color(prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Background"],      TextEditor.Margin.FolderBodyStyle.Background);

    TextEditor.Indent.TabSize               = prefs["TextEditor"]["Indent"].get("TabSize",               TextEditor.Indent.TabSize              ).asInt();
    TextEditor.Indent.ConvertTabsToSpaces   = prefs["TextEditor"]["Indent"].get("ConvertTabsToSpaces",   TextEditor.Indent.ConvertTabsToSpaces  ).asBool();
    TextEditor.Indent.IndentSize            = prefs["TextEditor"]["Indent"].get("IndentSize",            TextEditor.Indent.IndentSize           ).asInt();
    TextEditor.Indent.EnableAutoIndent      = prefs["TextEditor"]["Indent"].get("EnableAutoIndent",      TextEditor.Indent.EnableAutoIndent     ).asBool();
    TextEditor.Indent.BackspaceUnindents    = prefs["TextEditor"]["Indent"].get("BackspaceUnindents",    TextEditor.Indent.BackspaceUnindents   ).asBool();
    TextEditor.Indent.ShowIndentationGuides = prefs["TextEditor"]["Indent"].get("ShowIndentationGuides", TextEditor.Indent.ShowIndentationGuides).asBool();
    TextEditor.Indent.IndentationGuideStyle.Foreground = get_color(prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Foreground"],  TextEditor.Indent.IndentationGuideStyle.Foreground);
    TextEditor.Indent.IndentationGuideStyle.Background = get_color(prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Background"],  TextEditor.Indent.IndentationGuideStyle.Background);

    TextEditor.Braces.EnableBraceMatching       =           prefs["TextEditor"]["Braces"].get("EnableBraceMatching", TextEditor.Braces.EnableBraceMatching).asBool();
    TextEditor.Braces.HighlightStyle.Bold       =           prefs["TextEditor"]["Braces"]["HighlightStyle"].get("Bold",    TextEditor.Braces.HighlightStyle.Bold  ).asBool();
    TextEditor.Braces.HighlightStyle.Italic     =           prefs["TextEditor"]["Braces"]["HighlightStyle"].get("Italic",  TextEditor.Braces.HighlightStyle.Italic).asBool();
    TextEditor.Braces.HighlightStyle.Foreground = get_color(prefs["TextEditor"]["Braces"]["HighlightStyle"]["Foreground"], TextEditor.Braces.HighlightStyle.Foreground);
    TextEditor.Braces.HighlightStyle.Background = get_color(prefs["TextEditor"]["Braces"]["HighlightStyle"]["Background"], TextEditor.Braces.HighlightStyle.Background);
    TextEditor.Braces.BadlightStyle.Bold        =           prefs["TextEditor"]["Braces"]["BadlightStyle"].get("Bold",     TextEditor.Braces.BadlightStyle.Bold  ).asBool();
    TextEditor.Braces.BadlightStyle.Italic      =           prefs["TextEditor"]["Braces"]["BadlightStyle"].get("Italic",   TextEditor.Braces.BadlightStyle.Italic).asBool();
    TextEditor.Braces.BadlightStyle.Foreground  = get_color(prefs["TextEditor"]["Braces"]["BadlightStyle"]["Foreground"],  TextEditor.Braces.BadlightStyle.Foreground);
    TextEditor.Braces.BadlightStyle.Background  = get_color(prefs["TextEditor"]["Braces"]["BadlightStyle"]["Background"],  TextEditor.Braces.BadlightStyle.Background);

    TextEditor.Syntax.Operator.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Operator"]["Style"].get("Bold",    TextEditor.Syntax.Operator.Style.Bold  ).asBool();
    TextEditor.Syntax.Operator.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Operator"]["Style"].get("Italic",  TextEditor.Syntax.Operator.Style.Italic).asBool();
    TextEditor.Syntax.Operator.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Foreground"], TextEditor.Syntax.Operator.Style.Foreground);
    TextEditor.Syntax.Operator.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Background"], TextEditor.Syntax.Operator.Style.Background);

    TextEditor.Syntax.String.Style.Bold       =           prefs["TextEditor"]["Syntax"]["String"]["Style"].get("Bold",    TextEditor.Syntax.String.Style.Bold  ).asBool();
    TextEditor.Syntax.String.Style.Italic     =           prefs["TextEditor"]["Syntax"]["String"]["Style"].get("Italic",  TextEditor.Syntax.String.Style.Italic).asBool();
    TextEditor.Syntax.String.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["String"]["Style"]["Foreground"], TextEditor.Syntax.String.Style.Foreground);
    TextEditor.Syntax.String.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["String"]["Style"]["Background"], TextEditor.Syntax.String.Style.Background);

    TextEditor.Syntax.StringEOL.Style.Bold       =           prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"].get("Bold",    TextEditor.Syntax.StringEOL.Style.Bold  ).asBool();
    TextEditor.Syntax.StringEOL.Style.Italic     =           prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"].get("Italic",  TextEditor.Syntax.StringEOL.Style.Italic).asBool();
    TextEditor.Syntax.StringEOL.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Foreground"], TextEditor.Syntax.StringEOL.Style.Foreground);
    TextEditor.Syntax.StringEOL.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Background"], TextEditor.Syntax.StringEOL.Style.Background);

    TextEditor.Syntax.Identifier.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Identifier"]["Style"].get("Bold",    TextEditor.Syntax.Identifier.Style.Bold  ).asBool();
    TextEditor.Syntax.Identifier.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Identifier"]["Style"].get("Italic",  TextEditor.Syntax.Identifier.Style.Italic).asBool();
    TextEditor.Syntax.Identifier.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Foreground"], TextEditor.Syntax.Identifier.Style.Foreground);
    TextEditor.Syntax.Identifier.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Background"], TextEditor.Syntax.Identifier.Style.Background);

    TextEditor.Syntax.Number.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Number"]["Style"].get("Bold",    TextEditor.Syntax.Number.Style.Bold  ).asBool();
    TextEditor.Syntax.Number.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Number"]["Style"].get("Italic",  TextEditor.Syntax.Number.Style.Italic).asBool();
    TextEditor.Syntax.Number.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Foreground"], TextEditor.Syntax.Number.Style.Foreground);
    TextEditor.Syntax.Number.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Background"], TextEditor.Syntax.Number.Style.Background);

    TextEditor.Syntax.Character.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Character"]["Style"].get("Bold",    TextEditor.Syntax.Character.Style.Bold  ).asBool();
    TextEditor.Syntax.Character.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Character"]["Style"].get("Italic",  TextEditor.Syntax.Character.Style.Italic).asBool();
    TextEditor.Syntax.Character.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Foreground"], TextEditor.Syntax.Character.Style.Foreground);
    TextEditor.Syntax.Character.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Background"], TextEditor.Syntax.Character.Style.Background);

    TextEditor.Syntax.Comment.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Comment"]["Style"].get("Bold",    TextEditor.Syntax.Comment.Style.Bold  ).asBool();
    TextEditor.Syntax.Comment.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Comment"]["Style"].get("Italic",  TextEditor.Syntax.Comment.Style.Italic).asBool();
    TextEditor.Syntax.Comment.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Foreground"], TextEditor.Syntax.Comment.Style.Foreground);
    TextEditor.Syntax.Comment.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Background"], TextEditor.Syntax.Comment.Style.Background);

    TextEditor.Syntax.Keyword1.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"].get("Bold",    TextEditor.Syntax.Keyword1.Style.Bold  ).asBool();
    TextEditor.Syntax.Keyword1.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"].get("Italic",  TextEditor.Syntax.Keyword1.Style.Italic).asBool();
    TextEditor.Syntax.Keyword1.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Foreground"], TextEditor.Syntax.Keyword1.Style.Foreground);
    TextEditor.Syntax.Keyword1.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Background"], TextEditor.Syntax.Keyword1.Style.Background);

    TextEditor.Syntax.Keyword1.List.Clear();
    val = prefs["TextEditor"]["Syntax"]["Keyword1"]["List"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                TextEditor.Syntax.Keyword1.List.Add(val[i].asString());
            }
        }
    }

    TextEditor.Syntax.Keyword2.Style.Bold       =           prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"].get("Bold",    TextEditor.Syntax.Keyword2.Style.Bold  ).asBool();
    TextEditor.Syntax.Keyword2.Style.Italic     =           prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"].get("Italic",  TextEditor.Syntax.Keyword2.Style.Italic).asBool();
    TextEditor.Syntax.Keyword2.Style.Foreground = get_color(prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Foreground"], TextEditor.Syntax.Keyword2.Style.Foreground);
    TextEditor.Syntax.Keyword2.Style.Background = get_color(prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Background"], TextEditor.Syntax.Keyword2.Style.Background);

    TextEditor.Syntax.Keyword2.List.Clear();
    val = prefs["TextEditor"]["Syntax"]["Keyword2"]["List"];
    if (val.isArray() && val.size() > 0) {
        for (Json::ArrayIndex i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                TextEditor.Syntax.Keyword2.List.Add(val[i].asString());
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------
bool
Preferences::saveToFile(const std::string& fileName)
{
    // this will hold the preferences in json format
    Json::Value prefs;

    // write system preferences
    prefs["General"]["Window"]["Width"]       = General.Window.Width;
    prefs["General"]["Window"]["Height"]      = General.Window.Height;
    prefs["General"]["Window"]["X"]           = General.Window.X;
    prefs["General"]["Window"]["Y"]           = General.Window.Y;
    prefs["General"]["Window"]["Maximized"]   = General.Window.Maximized;

    prefs["General"]["FileTypeAssociation"]["Project"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < General.FileTypeAssociation.Project.size(); i++) {
        prefs["General"]["FileTypeAssociation"]["Project"][i] = General.FileTypeAssociation.Project[i].ToStdString();
    }

    prefs["General"]["FileTypeAssociation"]["Script"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < General.FileTypeAssociation.Script.size(); i++) {
        prefs["General"]["FileTypeAssociation"]["Script"][i] = General.FileTypeAssociation.Script[i].ToStdString();
    }

    prefs["General"]["FileTypeAssociation"]["Text"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < General.FileTypeAssociation.Text.size(); i++) {
        prefs["General"]["FileTypeAssociation"]["Text"][i] = General.FileTypeAssociation.Text[i].ToStdString();
    }

    prefs["General"]["DataLocation"]       = General.DataLocation.ToStdString();
    prefs["General"]["CommonDataLocation"] = General.CommonDataLocation.ToStdString();

    prefs["General"]["OpenLastProjectOnStartup"] = General.OpenLastProjectOnStartup;

    prefs["General"]["RecentFiles"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < General.RecentFiles.size(); i++) {
        prefs["General"]["RecentFiles"][i] = General.RecentFiles[i].ToStdString();
    }

    prefs["General"]["RecentProjects"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < General.RecentProjects.size(); i++) {
        prefs["General"]["RecentProjects"][i] = General.RecentProjects[i].ToStdString();
    }

    prefs["General"]["DockManagerState"] = General.DockManagerState.ToStdString();

    // write text editor preferences
    prefs["TextEditor"]["General"]["DefaultStyle"]["Foreground"]["Red"]   = TextEditor.General.DefaultStyle.Foreground.Red();
    prefs["TextEditor"]["General"]["DefaultStyle"]["Foreground"]["Green"] = TextEditor.General.DefaultStyle.Foreground.Green();
    prefs["TextEditor"]["General"]["DefaultStyle"]["Foreground"]["Blue"]  = TextEditor.General.DefaultStyle.Foreground.Blue();
    prefs["TextEditor"]["General"]["DefaultStyle"]["Background"]["Red"]   = TextEditor.General.DefaultStyle.Background.Red();
    prefs["TextEditor"]["General"]["DefaultStyle"]["Background"]["Green"] = TextEditor.General.DefaultStyle.Background.Green();
    prefs["TextEditor"]["General"]["DefaultStyle"]["Background"]["Blue"]  = TextEditor.General.DefaultStyle.Background.Blue();

    prefs["TextEditor"]["General"]["Caret"]["Width"]          = TextEditor.General.Caret.Width;
    prefs["TextEditor"]["General"]["Caret"]["Color"]["Red"]   = TextEditor.General.Caret.Color.Red();
    prefs["TextEditor"]["General"]["Caret"]["Color"]["Green"] = TextEditor.General.Caret.Color.Green();
    prefs["TextEditor"]["General"]["Caret"]["Color"]["Blue"]  = TextEditor.General.Caret.Color.Blue();

    prefs["TextEditor"]["General"]["Font"]["FaceName"]  = TextEditor.General.Font.FaceName.ToStdString();
    prefs["TextEditor"]["General"]["Font"]["PointSize"] = TextEditor.General.Font.PointSize;

    prefs["TextEditor"]["General"]["InitialZoomLevel"] = TextEditor.General.InitialZoomLevel;

    prefs["TextEditor"]["General"]["EnableAutoComplete"] = TextEditor.General.EnableAutoComplete;
    prefs["TextEditor"]["General"]["EnableCallTips"]     = TextEditor.General.EnableCallTips;

    prefs["TextEditor"]["Margin"]["DisplayLineNumbers"]                     = TextEditor.Margin.DisplayLineNumbers;
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Bold"]                = TextEditor.Margin.LineNumberStyle.Bold;
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Italic"]              = TextEditor.Margin.LineNumberStyle.Italic;
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Foreground"]["Red"]   = TextEditor.Margin.LineNumberStyle.Foreground.Red();
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Foreground"]["Green"] = TextEditor.Margin.LineNumberStyle.Foreground.Green();
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Foreground"]["Blue"]  = TextEditor.Margin.LineNumberStyle.Foreground.Blue();
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Background"]["Red"]   = TextEditor.Margin.LineNumberStyle.Background.Red();
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Background"]["Green"] = TextEditor.Margin.LineNumberStyle.Background.Green();
    prefs["TextEditor"]["Margin"]["LineNumberStyle"]["Background"]["Blue"]  = TextEditor.Margin.LineNumberStyle.Background.Blue();

    prefs["TextEditor"]["Margin"]["DisplayFolders"]   = TextEditor.Margin.DisplayFolders;
    prefs["TextEditor"]["Margin"]["FoldAllOnStartup"] = TextEditor.Margin.FoldAllOnStartup;

    prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Foreground"]["Red"]      = TextEditor.Margin.FolderMarginStyle.Foreground.Red();
    prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Foreground"]["Green"]    = TextEditor.Margin.FolderMarginStyle.Foreground.Green();
    prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Foreground"]["Blue"]     = TextEditor.Margin.FolderMarginStyle.Foreground.Blue();
    prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Background"]["Red"]      = TextEditor.Margin.FolderMarginStyle.Background.Red();
    prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Background"]["Green"]    = TextEditor.Margin.FolderMarginStyle.Background.Green();
    prefs["TextEditor"]["Margin"]["FolderMarginStyle"]["Background"]["Blue"]     = TextEditor.Margin.FolderMarginStyle.Background.Blue();

    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Shape"]                  = TextEditor.Margin.FolderHeaderStyle.Shape;
    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Foreground"]["Red"]      = TextEditor.Margin.FolderHeaderStyle.Foreground.Red();
    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Foreground"]["Green"]    = TextEditor.Margin.FolderHeaderStyle.Foreground.Green();
    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Foreground"]["Blue"]     = TextEditor.Margin.FolderHeaderStyle.Foreground.Blue();
    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Background"]["Red"]      = TextEditor.Margin.FolderHeaderStyle.Background.Red();
    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Background"]["Green"]    = TextEditor.Margin.FolderHeaderStyle.Background.Green();
    prefs["TextEditor"]["Margin"]["FolderHeaderStyle"]["Background"]["Blue"]     = TextEditor.Margin.FolderHeaderStyle.Background.Blue();

    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Shape"]               = TextEditor.Margin.FolderMidHeaderStyle.Shape;
    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Foreground"]["Red"]   = TextEditor.Margin.FolderMidHeaderStyle.Foreground.Red();
    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Foreground"]["Green"] = TextEditor.Margin.FolderMidHeaderStyle.Foreground.Green();
    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Foreground"]["Blue"]  = TextEditor.Margin.FolderMidHeaderStyle.Foreground.Blue();
    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Background"]["Red"]   = TextEditor.Margin.FolderMidHeaderStyle.Background.Red();
    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Background"]["Green"] = TextEditor.Margin.FolderMidHeaderStyle.Background.Green();
    prefs["TextEditor"]["Margin"]["FolderMidHeaderStyle"]["Background"]["Blue"]  = TextEditor.Margin.FolderMidHeaderStyle.Background.Blue();

    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Shape"]                    = TextEditor.Margin.FolderBodyStyle.Shape;
    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Foreground"]["Red"]        = TextEditor.Margin.FolderBodyStyle.Foreground.Red();
    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Foreground"]["Green"]      = TextEditor.Margin.FolderBodyStyle.Foreground.Green();
    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Foreground"]["Blue"]       = TextEditor.Margin.FolderBodyStyle.Foreground.Blue();
    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Background"]["Red"]        = TextEditor.Margin.FolderBodyStyle.Background.Red();
    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Background"]["Green"]      = TextEditor.Margin.FolderBodyStyle.Background.Green();
    prefs["TextEditor"]["Margin"]["FolderBodyStyle"]["Background"]["Blue"]       = TextEditor.Margin.FolderBodyStyle.Background.Blue();

    prefs["TextEditor"]["Indent"]["TabSize"]               = TextEditor.Indent.TabSize;
    prefs["TextEditor"]["Indent"]["ConvertTabsToSpaces"]   = TextEditor.Indent.ConvertTabsToSpaces;
    prefs["TextEditor"]["Indent"]["IndentSize"]            = TextEditor.Indent.IndentSize;
    prefs["TextEditor"]["Indent"]["EnableAutoIndent"]      = TextEditor.Indent.EnableAutoIndent;
    prefs["TextEditor"]["Indent"]["BackspaceUnindents"]    = TextEditor.Indent.BackspaceUnindents;
    prefs["TextEditor"]["Indent"]["ShowIndentationGuides"] = TextEditor.Indent.ShowIndentationGuides;
    prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Foreground"]["Red"]   = TextEditor.Indent.IndentationGuideStyle.Foreground.Red();
    prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Foreground"]["Green"] = TextEditor.Indent.IndentationGuideStyle.Foreground.Green();
    prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Foreground"]["Blue"]  = TextEditor.Indent.IndentationGuideStyle.Foreground.Blue();
    prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Background"]["Red"]   = TextEditor.Indent.IndentationGuideStyle.Background.Red();
    prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Background"]["Green"] = TextEditor.Indent.IndentationGuideStyle.Background.Green();
    prefs["TextEditor"]["Indent"]["IndentationGuideStyle"]["Background"]["Blue"]  = TextEditor.Indent.IndentationGuideStyle.Background.Blue();

    prefs["TextEditor"]["Braces"]["EnableBraceMatching"]                   = TextEditor.Braces.EnableBraceMatching;
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Bold"]                = TextEditor.Braces.HighlightStyle.Bold;
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Italic"]              = TextEditor.Braces.HighlightStyle.Italic;
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Foreground"]["Red"]   = TextEditor.Braces.HighlightStyle.Foreground.Red();
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Foreground"]["Green"] = TextEditor.Braces.HighlightStyle.Foreground.Green();
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Foreground"]["Blue"]  = TextEditor.Braces.HighlightStyle.Foreground.Blue();
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Background"]["Red"]   = TextEditor.Braces.HighlightStyle.Background.Red();
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Background"]["Green"] = TextEditor.Braces.HighlightStyle.Background.Green();
    prefs["TextEditor"]["Braces"]["HighlightStyle"]["Background"]["Blue"]  = TextEditor.Braces.HighlightStyle.Background.Blue();
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Bold"]                 = TextEditor.Braces.BadlightStyle.Bold;
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Italic"]               = TextEditor.Braces.BadlightStyle.Italic;
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Foreground"]["Red"]    = TextEditor.Braces.BadlightStyle.Foreground.Red();
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Foreground"]["Green"]  = TextEditor.Braces.BadlightStyle.Foreground.Green();
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Foreground"]["Blue"]   = TextEditor.Braces.BadlightStyle.Foreground.Blue();
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Background"]["Red"]    = TextEditor.Braces.BadlightStyle.Background.Red();
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Background"]["Green"]  = TextEditor.Braces.BadlightStyle.Background.Green();
    prefs["TextEditor"]["Braces"]["BadlightStyle"]["Background"]["Blue"]   = TextEditor.Braces.BadlightStyle.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Bold"]                = TextEditor.Syntax.Operator.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Italic"]              = TextEditor.Syntax.Operator.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Operator.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Operator.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Operator.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Operator.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Operator.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Operator"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Operator.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Bold"]                = TextEditor.Syntax.String.Style.Bold;
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Italic"]              = TextEditor.Syntax.String.Style.Italic;
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.String.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.String.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.String.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.String.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Background"]["Green"] = TextEditor.Syntax.String.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["String"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.String.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Bold"]                = TextEditor.Syntax.StringEOL.Style.Bold;
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Italic"]              = TextEditor.Syntax.StringEOL.Style.Italic;
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.StringEOL.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.StringEOL.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.StringEOL.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.StringEOL.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Background"]["Green"] = TextEditor.Syntax.StringEOL.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["StringEOL"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.StringEOL.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Bold"]                = TextEditor.Syntax.Identifier.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Italic"]              = TextEditor.Syntax.Identifier.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Identifier.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Identifier.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Identifier.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Identifier.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Identifier.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Identifier"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Identifier.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Bold"]                = TextEditor.Syntax.Number.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Italic"]              = TextEditor.Syntax.Number.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Number.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Number.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Number.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Number.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Number.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Number"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Number.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Bold"]                = TextEditor.Syntax.Character.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Italic"]              = TextEditor.Syntax.Character.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Character.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Character.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Character.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Character.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Character.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Character"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Character.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Bold"]                = TextEditor.Syntax.Comment.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Italic"]              = TextEditor.Syntax.Comment.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Comment.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Comment.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Comment.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Comment.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Comment.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Comment"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Comment.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Bold"]                = TextEditor.Syntax.Keyword1.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Italic"]              = TextEditor.Syntax.Keyword1.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Keyword1.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Keyword1.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Keyword1.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Keyword1.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Keyword1.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Keyword1"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Keyword1.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Keyword1"]["List"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < TextEditor.Syntax.Keyword1.List.size(); i++) {
        prefs["TextEditor"]["Syntax"]["Keyword1"]["List"][i] = TextEditor.Syntax.Keyword1.List[i].ToStdString();
    }

    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Bold"]                = TextEditor.Syntax.Keyword2.Style.Bold;
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Italic"]              = TextEditor.Syntax.Keyword2.Style.Italic;
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Foreground"]["Red"]   = TextEditor.Syntax.Keyword2.Style.Foreground.Red();
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Foreground"]["Green"] = TextEditor.Syntax.Keyword2.Style.Foreground.Green();
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Foreground"]["Blue"]  = TextEditor.Syntax.Keyword2.Style.Foreground.Blue();
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Background"]["Red"]   = TextEditor.Syntax.Keyword2.Style.Background.Red();
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Background"]["Green"] = TextEditor.Syntax.Keyword2.Style.Background.Green();
    prefs["TextEditor"]["Syntax"]["Keyword2"]["Style"]["Background"]["Blue"]  = TextEditor.Syntax.Keyword2.Style.Background.Blue();

    prefs["TextEditor"]["Syntax"]["Keyword2"]["List"] = Json::Value(Json::arrayValue);
    for (Json::ArrayIndex i = 0; i < TextEditor.Syntax.Keyword2.List.size(); i++) {
        prefs["TextEditor"]["Syntax"]["Keyword2"]["List"][i] = TextEditor.Syntax.Keyword2.List[i].ToStdString();
    }

    // write the preferences in json format to file
    std::ofstream prefs_file(fileName.c_str());
    prefs_file << prefs;

    return true;
}
