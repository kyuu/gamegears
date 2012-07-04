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
#include "../third-party/json/json.h"
#include "ProjectProperties.hpp"


//-------------------------------------------------------------------
ProjectProperties::ProjectProperties()
{
    loadDefaults();
}

//-------------------------------------------------------------------
ProjectProperties::ProjectProperties(const ProjectProperties& that)
{
    Name        = that.Name;
    Description = that.Description;
    EntryPoint  = that.EntryPoint;

    Window.Width      = that.Window.Width;
    Window.Height     = that.Window.Height;
    Window.FullScreen = that.Window.FullScreen;
}

//-------------------------------------------------------------------
ProjectProperties::~ProjectProperties()
{
}

//-------------------------------------------------------------------
ProjectProperties&
ProjectProperties::operator=(const ProjectProperties& that)
{
    Name        = that.Name;
    Description = that.Description;
    EntryPoint  = that.EntryPoint;

    Window.Width      = that.Window.Width;
    Window.Height     = that.Window.Height;
    Window.FullScreen = that.Window.FullScreen;

    return *this;
}

//-------------------------------------------------------------------
void
ProjectProperties::loadDefaults()
{
    Name        = "Unnamed Project";
    Description = "";
    EntryPoint  = "game.ggs";

    Window.Width      = 320;
    Window.Height     = 240;
    Window.FullScreen = false;
}

//-------------------------------------------------------------------
bool
ProjectProperties::loadFromFile(const std::string& fileName)
{
    std::fstream file;
    Json::Value  properties;
    Json::Reader reader;

    // open file
    file.open(fileName.c_str(), std::fstream::in);
    if (!file.is_open()) {
        return false;
    }

    // parse file
    if (!reader.parse(file, properties)) {
        return false;
    }

    // load properties
    Name        = properties.get("Name",        "Unnamed Project" ).asString();
    Description = properties.get("Description", ""                ).asString();
    EntryPoint  = properties.get("EntryPoint",  "game.ggs"        ).asString();

    Window.Width      = properties["Window"].get("Width",      320  ).asInt();
    Window.Height     = properties["Window"].get("Height",     240  ).asInt();
    Window.FullScreen = properties["Window"].get("FullScreen", false).asBool();

    return true;
}

//-------------------------------------------------------------------
bool
ProjectProperties::saveToFile(const std::string& fileName)
{
    std::fstream file;
    Json::Value  properties;
    Json::Reader reader;

    // try to preserve any additional content
    file.open(fileName.c_str(), std::fstream::in);
    if (file.is_open()) {
        reader.parse(file, properties);
        file.close();
    }

    // write properties
    properties["Name"]        = Name;
    properties["Description"] = Description;
    properties["EntryPoint"]  = EntryPoint;

    properties["Window"]["Width"]      = Window.Width;
    properties["Window"]["Height"]     = Window.Height;
    properties["Window"]["FullScreen"] = Window.FullScreen;

    // save file
    file.open(fileName.c_str(), std::fstream::out);
    file << properties;

    return true;
}
