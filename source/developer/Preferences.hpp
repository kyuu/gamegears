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

#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <string>
#include <vector>
#include <wx/wx.h>


enum {
    TE_FOLDER_HEADER_STYLE_ARROW = 0,
    TE_FOLDER_HEADER_STYLE_PLUS_MINUS,
    TE_FOLDER_HEADER_STYLE_PLUS_MINUS_BOX,
    TE_FOLDER_HEADER_STYLE_PLUS_MINUS_CIRCLE,
};

enum {
    TE_FOLDER_BODY_STYLE_EMPTY = 0,
    TE_FOLDER_BODY_STYLE_LINE,
    TE_FOLDER_BODY_STYLE_CURVED_LINE,
};


class Preferences {
public:
    Preferences();
    Preferences(const Preferences& that);
    ~Preferences();
    Preferences& operator=(const Preferences& that);

    void loadDefaults();
    bool loadFromFile(const std::string& fileName);
    bool saveToFile(const std::string& fileName);

    struct {
        struct {
            int Width;
            int Height;
            int X;
            int Y;
            bool Maximized;

        } Window;

        struct {
            wxArrayString Project;
            wxArrayString Script;
            wxArrayString Text;

        } FileTypeAssociation;

        wxString DataLocation;
        wxString CommonDataLocation;

        bool OpenLastProjectOnStartup;

        wxArrayString RecentFiles;
        wxArrayString RecentProjects;

        wxString DockManagerState;

    } General;

    struct {
        struct {
            struct {
                wxString FaceName;
                int PointSize;

            } Font;

            int InitialZoomLevel;

            bool EnableAutoComplete;
            bool EnableCallTips;

        } General;

        struct {
            bool DisplayLineNumbers;

            struct {
                bool Bold;
                bool Italic;
                wxColour Foreground;
                wxColour Background;

            } LineNumberStyle;

            bool DisplayFolders;
            bool FoldAllOnStartup;

            struct {
                int      Shape;
                wxColour Foreground;
                wxColour Background;

            } FolderHeaderStyle;

            struct {
                int      Shape;
                wxColour Foreground;
                wxColour Background;

            } FolderMidHeaderStyle;

            struct {
                int      Shape;
                wxColour Foreground;
                wxColour Background;

            } FolderBodyStyle;

        } Margin;

        struct {
            int  TabSize;
            bool ConvertTabsToSpaces;

            int  IndentSize;
            bool EnableAutoIndent;
            bool BackspaceUnindents;
            bool ShowIndentationGuides;

        } Indent;

        struct {
            bool EnableBraceMatching;

            struct {
                bool Bold;
                bool Italic;
                wxColour Foreground;
                wxColour Background;

            } HighlightStyle;

            struct {
                bool Bold;
                bool Italic;
                wxColour Foreground;
                wxColour Background;

            } BadlightStyle;

        } Braces;

        struct {
            bool EnableSyntaxHighlighting;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

            } Operator;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

            } String;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

            } Identifier;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

            } Number;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

            } Character;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

            } Comment;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

                wxArrayString List;

            } Keyword1;

            struct {
                struct {
                    bool Bold;
                    bool Italic;
                    wxColour Foreground;
                    wxColour Background;

                } Style;

                wxArrayString List;

            } Keyword2;

        } Syntax;

    } TextEditor;

};


#endif
