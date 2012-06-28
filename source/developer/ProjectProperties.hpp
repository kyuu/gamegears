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

#ifndef PROJECTPROPERTIES_HPP
#define PROJECTPROPERTIES_HPP

#include <string>


class ProjectProperties {
public:
    ProjectProperties();
    ProjectProperties(const ProjectProperties& that);
    ~ProjectProperties();
    ProjectProperties& operator=(const ProjectProperties& that);

    void loadDefaults();
    bool loadFromFile(const std::string& fileName);
    bool saveToFile(const std::string& fileName);

    struct {
        int Width;
        int Height;
        bool FullScreen;

    } Window;

    std::string Name;
    std::string Description;
    std::string EntryPoint;

};


#endif
