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

#ifndef COMMANDS_HPP
#define COMMANDS_HPP


enum {
    // developer
    CMD_EXIT_DEVELOPER = 0,
    CMD_SHOW_ABOUT_DEVELOPER,
    CMD_EDIT_DEVELOPER_PREFERENCES,

    // panes
    CMD_SHOW_PROJECT_DATA_EXPLORER,
    CMD_SHOW_COMMON_DATA_EXPLORER,
    CMD_SHOW_OUTPUT,

    // project
    CMD_CREATE_NEW_PROJECT,
    CMD_OPEN_PROJECT,
    CMD_OPEN_RECENT_PROJECT_1,
    CMD_OPEN_RECENT_PROJECT_2,
    CMD_OPEN_RECENT_PROJECT_3,
    CMD_OPEN_RECENT_PROJECT_4,
    CMD_OPEN_RECENT_PROJECT_5,
    CMD_OPEN_RECENT_PROJECT_6,
    CMD_OPEN_RECENT_PROJECT_7,
    CMD_OPEN_RECENT_PROJECT_8,
    CMD_OPEN_RECENT_PROJECT_9,
    CMD_OPEN_RECENT_PROJECT_10,
    CMD_PROJECT_ADD_NEW_FILE,
    CMD_START_PROJECT,
    CMD_PROJECT_FULLSCREEN,
    CMD_EDIT_PROJECT_PROPERTIES,
    CMD_CLOSE_PROJECT,

    // file explorer
    CMD_FILE_EXPLORER_REFRESH,
    CMD_FILE_EXPLORER_EXPAND,
    CMD_FILE_EXPLORER_COLLAPSE,
    CMD_FILE_EXPLORER_CREATE_NEW_DIRECTORY,
    CMD_FILE_EXPLORER_CREATE_NEW_FILE,
    CMD_FILE_EXPLORER_OPEN_FILE,
    CMD_FILE_EXPLORER_OPEN_FILE_AS_SCRIPT,
    CMD_FILE_EXPLORER_OPEN_FILE_AS_TEXT,
    CMD_FILE_EXPLORER_RENAME_FILE,
    CMD_FILE_EXPLORER_DELETE_FILE,

    // file
    CMD_CREATE_NEW_FILE,
    CMD_OPEN_FILE,
    CMD_OPEN_RECENT_FILE_1,
    CMD_OPEN_RECENT_FILE_2,
    CMD_OPEN_RECENT_FILE_3,
    CMD_OPEN_RECENT_FILE_4,
    CMD_OPEN_RECENT_FILE_5,
    CMD_OPEN_RECENT_FILE_6,
    CMD_OPEN_RECENT_FILE_7,
    CMD_OPEN_RECENT_FILE_8,
    CMD_OPEN_RECENT_FILE_9,
    CMD_OPEN_RECENT_FILE_10,
    CMD_SAVE_FILE,
    CMD_SAVE_FILE_AS,
    CMD_SAVE_ALL_FILES,
    CMD_CLOSE_FILE,
    CMD_CLOSE_ALL_FILES,

    // editor generic
    CMD_UNDO,
    CMD_REDO,
    CMD_CUT,
    CMD_COPY,
    CMD_PASTE,

    // text editor
    CMD_TEXT_ZOOM_IN,
    CMD_TEXT_ZOOM_OUT,
    CMD_TEXT_COMMENT_OUT,
    CMD_TEXT_UNCOMMENT,
    CMD_TEXT_INDENT,
    CMD_TEXT_OUTDENT,
    CMD_TEXT_MOVE_UP,
    CMD_TEXT_MOVE_DOWN,
    CMD_TEXT_GOTO,
};


#endif
