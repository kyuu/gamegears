#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <string>
#include <vector>


struct RgbColor {
    RgbColor() : Red(0), Green(0), Blue(0) { }
    RgbColor(int r, int g, int b) : Red(r), Green(g), Blue(b) { }
    RgbColor(const RgbColor& that) : Red(that.Red), Green(that.Green), Blue(that.Blue) { }
    ~RgbColor() { }
    RgbColor& operator=(const RgbColor& that) {
        Red   = that.Red;
        Green = that.Green;
        Blue  = that.Blue;
        return *this;
    }

    int Red;
    int Green;
    int Blue;
};


class Preferences {
public:
    Preferences();
    ~Preferences();

    void loadDefaults();
    bool load(const std::string& filename);
    bool save(const std::string& filename);

    struct {
        struct {
            int Width;
            int Height;
            int X;
            int Y;
            bool Maximized;

        } Window;

        std::string PathToData;
        std::string PathToCommonData;

        std::vector<std::string> RecentFiles;
        std::vector<std::string> RecentProjects;

        std::string PaneInfo;

    } System;

    struct {
        int ZoomLevel;

        // indentation
        struct {
            int  IndentSize;
            bool EnableAutoIndent;
            bool BackspaceUnindents;
            bool ShowIndentationGuides;

        } Indentation;

        // end of line
        bool ShowLineEndings;

        // whitespace
        bool ShowWhitespace;

        // line numbers
        struct {
            bool Enabled;
            int  MarginWidth;
            RgbColor Color;

        } LineNumbers;

        // folding
        struct {
            bool Enabled;
            int  MarginWidth;

        } Folding;

        // brace matching
        struct {
            bool Enabled;
            RgbColor  HighlightColor;
            RgbColor  BadlightColor;

        } BraceMatching;

        // auto-complete
        struct {
            bool Enabled;

        } AutoComplete;

        // calltips
        struct {
            bool Enabled;

        } CallTips;

        struct {
            struct {
                RgbColor Color;
                bool Bold;

            } String;

            struct {
                RgbColor Color;
                bool Bold;


            } Identifier;

            struct {
                RgbColor Color;
                bool Bold;

            } Number;

            struct {
                RgbColor Color;
                bool Bold;

            } Character;

            struct {
                RgbColor Color;
                bool Bold;

            } Comment;

            struct {
                RgbColor Color;
                bool Bold;
                std::vector<std::string> List;

            } PrimaryKeyword;

            struct {
                RgbColor Color;
                bool Bold;
                std::vector<std::string> List;

            } SecondaryKeyword;

        } SyntaxHighlighting;

    } ScriptEditor;

    struct {
        struct {
            std::vector<std::string> Project;
            std::vector<std::string> Script;
            std::vector<std::string> Text;
            std::vector<std::string> Image;
            std::vector<std::string> Audio;

        } FileTypes;

    } FileExplorer;

};


#endif
