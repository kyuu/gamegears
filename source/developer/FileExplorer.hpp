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

#ifndef FILEEXPLORER_HPP
#define FILEEXPLORER_HPP

#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/treectrl.h>


// forward declaration
class Developer;


class FileExplorer : public wxTreeCtrl {
public:
    enum {
        ICON_REFRESH = 0,
        ICON_NEW_DIRECTORY,
        ICON_NEW_FILE,
        ICON_DELETE,
        ICON_DIRECTORY_CLOSED,
        ICON_DIRECTORY_OPEN,
        ICON_FILE_TYPE_UNKNOWN,
        ICON_FILE_TYPE_PROJECT,
        ICON_FILE_TYPE_SCRIPT,
        ICON_FILE_TYPE_TEXT,

        _ICON_COUNT, // not an icon
    };

    explicit FileExplorer(Developer* developer, const wxString& rootPath = wxEmptyString);
    ~FileExplorer();

    const wxString& getRootPath() const;
    void clearFileTree();
    void initFileTree(const wxString& rootPath);

    // invoked by the developer whenever a file was saved
    void fileSaved(const wxString& fileName);

private:
    bool removeAll(const wxString& fileName);
    wxString constructPathFromItem(wxTreeItemId item);
    bool isItemChildOf(wxTreeItemId possibleChild, wxTreeItemId possibleParent);
    void buildItemTree(wxTreeItemId item);
    void setItemIcons(wxTreeItemId item);
    wxMenu* createItemContextMenu(wxTreeItemId item);

    // event handlers
    void onItemBeginLabelEdit(wxTreeEvent& event);
    void onItemEndLabelEdit(wxTreeEvent& event);
    void onItemBeginDrag(wxTreeEvent& event);
    void onItemEndDrag(wxTreeEvent& event);
    void onItemRightClick(wxTreeEvent& event);
    void onItemActivated(wxTreeEvent& event);

    // context menu event handlers
    void onRefreshTree(wxCommandEvent& event);
    void onExpandTree(wxCommandEvent& event);
    void onCollapseTree(wxCommandEvent& event);
    void onAddNewDirectory(wxCommandEvent& event);
    void onAddNewFile(wxCommandEvent& event);
    void onOpenFile(wxCommandEvent& event);
    void onOpenFileAsScript(wxCommandEvent& event);
    void onOpenFileAsText(wxCommandEvent& event);
    void onRenameFile(wxCommandEvent& event);
    void onDeleteFile(wxCommandEvent& event);

private:
    Developer*       _developer;
    wxString         _rootPath;
    wxImageList*     _iconList;
    std::vector<int> _iconIndices;

    // used in a drag event
    wxTreeItemId _draggedItem;
};


#endif
