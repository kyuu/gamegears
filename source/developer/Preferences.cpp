#include <fstream>
#include <wx/wx.h>
#include "json/json.h"
#include "Preferences.hpp"

#define DEFAULT_DEVELOPERWINDOW_WIDTH  800
#define DEFAULT_DEVELOPERWINDOW_HEIGHT 600
#define DEFAULT_DEVELOPERWINDOW_POSX   wxDefaultCoord
#define DEFAULT_DEVELOPERWINDOW_POSY   wxDefaultCoord


//-------------------------------------------------------------------
static std::string DefaultPrimaryKeywords[] = {
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

static std::string DefaultSecondaryKeywords[] = {
    "", // end of keywords
};

static std::string DefaultProjectFileTypes[] = {
    "ggproj",
    "", // end of file types
};

static std::string DefaultScriptFileTypes[] = {
    "ggs",
    "nut",
    "sq",
    "", // end of file types
};

static std::string DefaultTextFileTypes[] = {
    "txt",
    "", // end of file types
};

static std::string DefaultImageFileTypes[] = {
    "bmp",
    "png",
    "jpg",
    "jpeg",
    "", // end of file types
};

static std::string DefaultAudioFileTypes[] = {
    "wav",
    "ogg",
    "flac",
    "spx",
    "mod",
    "it",
    "mp3",
    "", // end of file types
};

//-------------------------------------------------------------------
Preferences::Preferences()
{
    loadDefaults();
}

//-------------------------------------------------------------------
Preferences::~Preferences()
{
}

//-------------------------------------------------------------------
void
Preferences::loadDefaults()
{
    // load system defaults
    System.Window.Width     = DEFAULT_DEVELOPERWINDOW_WIDTH;
    System.Window.Height    = DEFAULT_DEVELOPERWINDOW_HEIGHT;
    System.Window.X         = DEFAULT_DEVELOPERWINDOW_POSX;
    System.Window.Y         = DEFAULT_DEVELOPERWINDOW_POSY;
    System.Window.Maximized = false;

    System.PathToData       = "data";
    System.PathToCommonData = "common";

    System.RecentFiles.clear();
    System.RecentProjects.clear();

    System.PaneInfo = "";

    // load script editor defaults
    ScriptEditor.ZoomLevel = 0;

    ScriptEditor.Indentation.IndentSize               = 4;
    ScriptEditor.Indentation.EnableAutoIndent         = true;
    ScriptEditor.Indentation.BackspaceUnindents       = true;
    ScriptEditor.Indentation.ShowIndentationGuides    = false;

    ScriptEditor.ShowLineEndings = false;

    ScriptEditor.ShowWhitespace = false;

    ScriptEditor.LineNumbers.Enabled     = true;
    ScriptEditor.LineNumbers.MarginWidth = 35;
    ScriptEditor.LineNumbers.Color       = RgbColor(75, 75, 75);

    ScriptEditor.Folding.Enabled     = true;
    ScriptEditor.Folding.MarginWidth = 15;

    ScriptEditor.BraceMatching.Enabled        = true;
    ScriptEditor.BraceMatching.HighlightColor = RgbColor(0, 128, 0);
    ScriptEditor.BraceMatching.BadlightColor  = RgbColor(255, 0, 0);

    ScriptEditor.AutoComplete.Enabled = false;

    ScriptEditor.CallTips.Enabled = false;

    ScriptEditor.SyntaxHighlighting.String.Color     = RgbColor(0, 128, 128);
    ScriptEditor.SyntaxHighlighting.String.Bold      = false;

    ScriptEditor.SyntaxHighlighting.Identifier.Color = RgbColor(0, 0, 0);
    ScriptEditor.SyntaxHighlighting.Identifier.Bold  = false;

    ScriptEditor.SyntaxHighlighting.Number.Color     = RgbColor(128, 0, 0);
    ScriptEditor.SyntaxHighlighting.Number.Bold      = false;

    ScriptEditor.SyntaxHighlighting.Character.Color  = RgbColor(128, 128, 0);
    ScriptEditor.SyntaxHighlighting.Character.Bold   = false;

    ScriptEditor.SyntaxHighlighting.Comment.Color    = RgbColor(0, 128, 0);
    ScriptEditor.SyntaxHighlighting.Comment.Bold     = false;


    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color = RgbColor(0, 0, 255);
    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Bold  = false;
    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List.clear();
    for (int i = 0; !DefaultPrimaryKeywords[i].empty(); i++) {
        ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List.push_back(DefaultPrimaryKeywords[i]);
    }

    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color = RgbColor(0, 0, 128);
    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Bold  = false;
    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.List.clear();
    for (int i = 0; !DefaultSecondaryKeywords[i].empty(); i++) {
        ScriptEditor.SyntaxHighlighting.SecondaryKeyword.List.push_back(DefaultSecondaryKeywords[i]);
    }

    // load project explorer defaults
    FileExplorer.FileTypes.Project.clear();
    for (int i = 0; !DefaultProjectFileTypes[i].empty(); i++) {
        FileExplorer.FileTypes.Project.push_back(DefaultProjectFileTypes[i]);
    }

    FileExplorer.FileTypes.Script.clear();
    for (int i = 0; !DefaultScriptFileTypes[i].empty(); i++) {
        FileExplorer.FileTypes.Script.push_back(DefaultScriptFileTypes[i]);
    }

    FileExplorer.FileTypes.Text.clear();
    for (int i = 0; !DefaultTextFileTypes[i].empty(); i++) {
        FileExplorer.FileTypes.Text.push_back(DefaultTextFileTypes[i]);
    }

    FileExplorer.FileTypes.Image.clear();
    for (int i = 0; !DefaultImageFileTypes[i].empty(); i++) {
        FileExplorer.FileTypes.Image.push_back(DefaultImageFileTypes[i]);
    }

    FileExplorer.FileTypes.Audio.clear();
    for (int i = 0; !DefaultAudioFileTypes[i].empty(); i++) {
        FileExplorer.FileTypes.Audio.push_back(DefaultAudioFileTypes[i]);
    }

}

//-------------------------------------------------------------------
bool
Preferences::load(const std::string& filename)
{
    // open configuration file
    std::ifstream prefs_file(filename.c_str());
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

    // load system preferences
    System.Window.Width     = prefs["System"]["Window"].get("Width",     DEFAULT_DEVELOPERWINDOW_WIDTH ).asInt();
    System.Window.Height    = prefs["System"]["Window"].get("Height",    DEFAULT_DEVELOPERWINDOW_HEIGHT).asInt();
    System.Window.X         = prefs["System"]["Window"].get("X",         DEFAULT_DEVELOPERWINDOW_POSX  ).asInt();
    System.Window.Y         = prefs["System"]["Window"].get("Y",         DEFAULT_DEVELOPERWINDOW_POSY  ).asInt();
    System.Window.Maximized = prefs["System"]["Window"].get("Maximized", false                         ).asBool();

    System.PathToData       = prefs["System"].get("PathToData",       "data"  ).asString();
    System.PathToCommonData = prefs["System"].get("PathToCommonData", "common").asString();

    System.RecentFiles.clear();
    val = prefs["System"]["RecentFiles"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                System.RecentFiles.push_back(val[i].asString());
            }
        }
    }

    System.RecentProjects.clear();
    val = prefs["System"]["RecentProjects"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                System.RecentProjects.push_back(val[i].asString());
            }
        }
    }

    System.PaneInfo = prefs["System"].get("PaneInfo", "").asString();

    // load script editor preferences
    ScriptEditor.ZoomLevel = prefs["ScriptEditor"].get("ZoomLevel", 0).asInt();

    ScriptEditor.Indentation.IndentSize            = prefs["ScriptEditor"]["Indentation"].get("IndentSize",            4    ).asInt();
    ScriptEditor.Indentation.EnableAutoIndent      = prefs["ScriptEditor"]["Indentation"].get("EnableAutoIndent",      true ).asBool();
    ScriptEditor.Indentation.BackspaceUnindents    = prefs["ScriptEditor"]["Indentation"].get("BackspaceUnindents",    true ).asBool();
    ScriptEditor.Indentation.ShowIndentationGuides = prefs["ScriptEditor"]["Indentation"].get("ShowIndentationGuides", false).asBool();

    ScriptEditor.ShowLineEndings = prefs["ScriptEditor"].get("ShowLineEndings", false).asBool();

    ScriptEditor.ShowWhitespace = prefs["ScriptEditor"].get("ShowWhitespace", false).asBool();

    ScriptEditor.LineNumbers.Enabled     = prefs["ScriptEditor"]["LineNumbers"].get("Enabled",     true).asBool();
    ScriptEditor.LineNumbers.MarginWidth = prefs["ScriptEditor"]["LineNumbers"].get("MarginWidth", 35  ).asInt();
    ScriptEditor.LineNumbers.Color.Red   = prefs["ScriptEditor"]["LineNumbers"]["Color"].get("Red",   75).asInt();
    ScriptEditor.LineNumbers.Color.Green = prefs["ScriptEditor"]["LineNumbers"]["Color"].get("Green", 75).asInt();
    ScriptEditor.LineNumbers.Color.Blue  = prefs["ScriptEditor"]["LineNumbers"]["Color"].get("Blue",  75).asInt();

    ScriptEditor.Folding.Enabled     = prefs["ScriptEditor"]["Folding"].get("Enabled",     true).asBool();
    ScriptEditor.Folding.MarginWidth = prefs["ScriptEditor"]["Folding"].get("MarginWidth", 15  ).asInt();

    ScriptEditor.BraceMatching.Enabled              = prefs["ScriptEditor"]["BraceMatching"].get("Enabled", true).asBool();
    ScriptEditor.BraceMatching.HighlightColor.Red   = prefs["ScriptEditor"]["BraceMatching"]["HighlightColor"].get("Red",   0  ).asInt();
    ScriptEditor.BraceMatching.HighlightColor.Green = prefs["ScriptEditor"]["BraceMatching"]["HighlightColor"].get("Green", 128).asInt();
    ScriptEditor.BraceMatching.HighlightColor.Blue  = prefs["ScriptEditor"]["BraceMatching"]["HighlightColor"].get("Blue",  0  ).asInt();
    ScriptEditor.BraceMatching.BadlightColor.Red    = prefs["ScriptEditor"]["BraceMatching"]["BadlightColor"].get("Red",   255).asInt();
    ScriptEditor.BraceMatching.BadlightColor.Green  = prefs["ScriptEditor"]["BraceMatching"]["BadlightColor"].get("Green", 0  ).asInt();
    ScriptEditor.BraceMatching.BadlightColor.Blue   = prefs["ScriptEditor"]["BraceMatching"]["BadlightColor"].get("Blue",  0  ).asInt();

    ScriptEditor.AutoComplete.Enabled = prefs["ScriptEditor"]["AutoComplete"].get("Enabled", false).asBool();

    ScriptEditor.CallTips.Enabled = prefs["ScriptEditor"]["CallTips"].get("Enabled", false).asBool();

    ScriptEditor.SyntaxHighlighting.String.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Color"].get("Red",   0  ).asInt();
    ScriptEditor.SyntaxHighlighting.String.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Color"].get("Green", 128).asInt();
    ScriptEditor.SyntaxHighlighting.String.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Color"].get("Blue",  128).asInt();
    ScriptEditor.SyntaxHighlighting.String.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["String"].get("Bold", false).asBool();

    ScriptEditor.SyntaxHighlighting.Identifier.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Color"].get("Red",   0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Identifier.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Color"].get("Green", 0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Identifier.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Color"].get("Blue",  0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Identifier.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"].get("Bold", false).asBool();

    ScriptEditor.SyntaxHighlighting.Number.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Color"].get("Red",   128).asInt();
    ScriptEditor.SyntaxHighlighting.Number.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Color"].get("Green", 0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Number.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Color"].get("Blue",  0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Number.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"].get("Bold", false).asBool();

    ScriptEditor.SyntaxHighlighting.Character.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Color"].get("Red",   128).asInt();
    ScriptEditor.SyntaxHighlighting.Character.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Color"].get("Green", 128).asInt();
    ScriptEditor.SyntaxHighlighting.Character.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Color"].get("Blue",  0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Character.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"].get("Bold", false).asBool();

    ScriptEditor.SyntaxHighlighting.Comment.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Color"].get("Red",   0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Comment.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Color"].get("Green", 128).asInt();
    ScriptEditor.SyntaxHighlighting.Comment.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Color"].get("Blue",  0  ).asInt();
    ScriptEditor.SyntaxHighlighting.Comment.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"].get("Bold", false).asBool();

    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Color"].get("Red",   0  ).asInt();
    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Color"].get("Green", 0  ).asInt();
    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Color"].get("Blue",  255).asInt();
    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"].get("Bold", false).asBool();
    ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List.clear();
    val = prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["List"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List.push_back(val[i].asString());
            }
        }
    }

    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color.Red   = prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Color"].get("Red",   0  ).asInt();
    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color.Green = prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Color"].get("Green", 0  ).asInt();
    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color.Blue  = prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Color"].get("Blue",  128).asInt();
    ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Bold        = prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"].get("Bold", false).asBool();
    val = prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["List"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                ScriptEditor.SyntaxHighlighting.SecondaryKeyword.List.push_back(val[i].asString());
            }
        }
    }

    // load project explorer preferences
    FileExplorer.FileTypes.Project.clear();
    val = prefs["FileExplorer"]["FileTypes"]["Project"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                FileExplorer.FileTypes.Project.push_back(val[i].asString());
            }
        }
    }

    FileExplorer.FileTypes.Script.clear();
    val = prefs["FileExplorer"]["FileTypes"]["Script"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                FileExplorer.FileTypes.Script.push_back(val[i].asString());
            }
        }
    }

    FileExplorer.FileTypes.Text.clear();
    val = prefs["FileExplorer"]["FileTypes"]["Text"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                FileExplorer.FileTypes.Text.push_back(val[i].asString());
            }
        }
    }

    FileExplorer.FileTypes.Image.clear();
    val = prefs["FileExplorer"]["FileTypes"]["Image"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                FileExplorer.FileTypes.Image.push_back(val[i].asString());
            }
        }
    }

    FileExplorer.FileTypes.Audio.clear();
    val = prefs["FileExplorer"]["FileTypes"]["Audio"];
    if (val.isArray() && val.size() > 0) {
        for (size_t i = 0; i < val.size(); i++) {
            if (val[i].isString()) {
                FileExplorer.FileTypes.Audio.push_back(val[i].asString());
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------
bool
Preferences::save(const std::string& filename)
{
    // this will hold the preferences in json format
    Json::Value prefs;

    // write system preferences
    prefs["System"]["Window"]["Width"]     = System.Window.Width;
    prefs["System"]["Window"]["Height"]    = System.Window.Height;
    prefs["System"]["Window"]["X"]         = System.Window.X;
    prefs["System"]["Window"]["Y"]         = System.Window.Y;
    prefs["System"]["Window"]["Maximized"] = System.Window.Maximized;

    prefs["System"]["RecentFiles"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < System.RecentFiles.size(); i++) {
        prefs["System"]["RecentFiles"][i] = System.RecentFiles[i];
    }

    prefs["System"]["RecentProjects"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < System.RecentProjects.size(); i++) {
        prefs["System"]["RecentProjects"][i] = System.RecentProjects[i];
    }

    prefs["System"]["PathToData"]       = System.PathToData;
    prefs["System"]["PathToCommonData"] = System.PathToCommonData;

    prefs["System"]["PaneInfo"] = System.PaneInfo;

    // write script editor preferences
    prefs["ScriptEditor"]["ZoomLevel"] = ScriptEditor.ZoomLevel;

    prefs["ScriptEditor"]["Indentation"]["IndentSize"]            = ScriptEditor.Indentation.IndentSize;
    prefs["ScriptEditor"]["Indentation"]["EnableAutoIndent"]      = ScriptEditor.Indentation.EnableAutoIndent;
    prefs["ScriptEditor"]["Indentation"]["BackspaceUnindents"]    = ScriptEditor.Indentation.BackspaceUnindents;
    prefs["ScriptEditor"]["Indentation"]["ShowIndentationGuides"] = ScriptEditor.Indentation.ShowIndentationGuides;

    prefs["ScriptEditor"]["ShowLineEndings"] = ScriptEditor.ShowLineEndings;

    prefs["ScriptEditor"]["ShowWhitespace"] = ScriptEditor.ShowWhitespace;

    prefs["ScriptEditor"]["LineNumbers"]["Enabled"]        = ScriptEditor.LineNumbers.Enabled;
    prefs["ScriptEditor"]["LineNumbers"]["MarginWidth"]    = ScriptEditor.LineNumbers.MarginWidth;
    prefs["ScriptEditor"]["LineNumbers"]["Color"]["Red"]   = ScriptEditor.LineNumbers.Color.Red;
    prefs["ScriptEditor"]["LineNumbers"]["Color"]["Green"] = ScriptEditor.LineNumbers.Color.Green;
    prefs["ScriptEditor"]["LineNumbers"]["Color"]["Blue"]  = ScriptEditor.LineNumbers.Color.Blue;

    prefs["ScriptEditor"]["Folding"]["Enabled"]     = ScriptEditor.Folding.Enabled;
    prefs["ScriptEditor"]["Folding"]["MarginWidth"] = ScriptEditor.Folding.MarginWidth;

    prefs["ScriptEditor"]["BraceMatching"]["Enabled"]                 = ScriptEditor.BraceMatching.Enabled;
    prefs["ScriptEditor"]["BraceMatching"]["HighlightColor"]["Red"]   = ScriptEditor.BraceMatching.HighlightColor.Red;
    prefs["ScriptEditor"]["BraceMatching"]["HighlightColor"]["Green"] = ScriptEditor.BraceMatching.HighlightColor.Green;
    prefs["ScriptEditor"]["BraceMatching"]["HighlightColor"]["Blue"]  = ScriptEditor.BraceMatching.HighlightColor.Blue;
    prefs["ScriptEditor"]["BraceMatching"]["BadlightColor"]["Red"]    = ScriptEditor.BraceMatching.BadlightColor.Red;
    prefs["ScriptEditor"]["BraceMatching"]["BadlightColor"]["Green"]  = ScriptEditor.BraceMatching.BadlightColor.Green;
    prefs["ScriptEditor"]["BraceMatching"]["BadlightColor"]["Blue"]   = ScriptEditor.BraceMatching.BadlightColor.Blue;

    prefs["ScriptEditor"]["AutoComplete"]["Enabled"] = ScriptEditor.AutoComplete.Enabled;

    prefs["ScriptEditor"]["CallTips"]["Enabled"] = ScriptEditor.CallTips.Enabled;

    prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.String.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.String.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.String.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["String"]["Bold"]            = ScriptEditor.SyntaxHighlighting.String.Bold;

    prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.Identifier.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.Identifier.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.Identifier.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Identifier"]["Bold"]            = ScriptEditor.SyntaxHighlighting.Identifier.Bold;

    prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.Number.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.Number.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.Number.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Number"]["Bold"]            = ScriptEditor.SyntaxHighlighting.Number.Bold;

    prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.Character.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.Character.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.Character.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Character"]["Bold"]            = ScriptEditor.SyntaxHighlighting.Character.Bold;

    prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.Comment.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.Comment.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.Comment.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["Comment"]["Bold"]            = ScriptEditor.SyntaxHighlighting.Comment.Bold;

    prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["Bold"]            = ScriptEditor.SyntaxHighlighting.PrimaryKeyword.Bold;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["List"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List.size(); i++) {
        prefs["ScriptEditor"]["SyntaxHighlighting"]["PrimaryKeyword"]["List"][i] = ScriptEditor.SyntaxHighlighting.PrimaryKeyword.List[i];
    }

    prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Color"]["Red"]    = ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color.Red;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Color"]["Green"]  = ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color.Green;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Color"]["Blue"]   = ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Color.Blue;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["Bold"]            = ScriptEditor.SyntaxHighlighting.SecondaryKeyword.Bold;
    prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["List"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < ScriptEditor.SyntaxHighlighting.SecondaryKeyword.List.size(); i++) {
        prefs["ScriptEditor"]["SyntaxHighlighting"]["SecondaryKeyword"]["List"][i] = ScriptEditor.SyntaxHighlighting.SecondaryKeyword.List[i];
    }

    // write project explorer preferences
    prefs["FileExplorer"]["FileTypes"]["Project"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < FileExplorer.FileTypes.Project.size(); i++) {
        prefs["FileExplorer"]["FileTypes"]["Project"][i] = FileExplorer.FileTypes.Project[i];
    }

    prefs["FileExplorer"]["FileTypes"]["Script"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < FileExplorer.FileTypes.Script.size(); i++) {
        prefs["FileExplorer"]["FileTypes"]["Script"][i] = FileExplorer.FileTypes.Script[i];
    }

    prefs["FileExplorer"]["FileTypes"]["Text"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < FileExplorer.FileTypes.Text.size(); i++) {
        prefs["FileExplorer"]["FileTypes"]["Text"][i] = FileExplorer.FileTypes.Text[i];
    }

    prefs["FileExplorer"]["FileTypes"]["Image"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < FileExplorer.FileTypes.Image.size(); i++) {
        prefs["FileExplorer"]["FileTypes"]["Image"][i] = FileExplorer.FileTypes.Image[i];
    }

    prefs["FileExplorer"]["FileTypes"]["Audio"] = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < FileExplorer.FileTypes.Audio.size(); i++) {
        prefs["FileExplorer"]["FileTypes"]["Audio"][i] = FileExplorer.FileTypes.Audio[i];
    }

    // write the preferences in json format to file
    std::ofstream prefs_file(filename.c_str());
    prefs_file << prefs;

    return true;
}
