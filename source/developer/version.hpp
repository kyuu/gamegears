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

#ifndef VERSION_HPP
#define VERSION_HPP


#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define BUILD_VERSION(major, minor, patch)  ((major << 24) | (minor << 12) | patch)
#define BUILD_VERSION_STRING(major, minor, patch)  (STRINGIFY(major) "." STRINGIFY(minor) "." STRINGIFY(patch))

#define DEVELOPER_MAJOR 0
#define DEVELOPER_MINOR 2
#define DEVELOPER_PATCH 0

#define DEVELOPER_VERSION        BUILD_VERSION(DEVELOPER_MAJOR, DEVELOPER_MINOR, DEVELOPER_PATCH)
#define DEVELOPER_VERSION_STRING BUILD_VERSION_STRING(DEVELOPER_MAJOR, DEVELOPER_MINOR, DEVELOPER_PATCH)


#endif
