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

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/treebase.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "FileExplorer.hpp"
#include "Developer.hpp"
#include "NewDirectoryDialog.hpp"
#include "commands.hpp"


//-------------------------------------------------------------------
struct TreeItemData : public wxTreeItemData {
    TreeItemData(int type) : Type(type) { }
    TreeItemData(const TreeItemData& that) : Type(that.Type) { }
    ~TreeItemData() { }

    TreeItemData& operator=(const TreeItemData& that) {
        Type = that.Type;
        return *this;
    }

    int Type;
};

//-------------------------------------------------------------------
FileExplorer::FileExplorer(Developer* developer, const wxString& rootPath)
    : wxTreeCtrl(developer, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS)
    , _developer(developer)
    , _rootPath(rootPath)
    , _iconList(0)
    , _iconIndices(_ICON_COUNT)
{
    // connect events
    Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(FileExplorer::onItemBeginLabelEdit));
    Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT,   wxTreeEventHandler(FileExplorer::onItemEndLabelEdit));
    Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG,       wxTreeEventHandler(FileExplorer::onItemBeginDrag));
    Connect(wxEVT_COMMAND_TREE_END_DRAG,         wxTreeEventHandler(FileExplorer::onItemEndDrag));
    Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(FileExplorer::onItemRightClick));
    Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED ,  wxTreeEventHandler(FileExplorer::onItemActivated));

    Connect(CMD_FILE_EXPLORER_REFRESH,              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onRefreshTree));
    Connect(CMD_FILE_EXPLORER_EXPAND,               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onExpandTree));
    Connect(CMD_FILE_EXPLORER_COLLAPSE,             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onCollapseTree));
    Connect(CMD_FILE_EXPLORER_CREATE_NEW_DIRECTORY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onAddNewDirectory));
    Connect(CMD_FILE_EXPLORER_CREATE_NEW_FILE,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onAddNewFile));
    Connect(CMD_FILE_EXPLORER_OPEN_FILE,            wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onOpenFile));
    Connect(CMD_FILE_EXPLORER_OPEN_FILE_AS_SCRIPT,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onOpenFileAsScript));
    Connect(CMD_FILE_EXPLORER_OPEN_FILE_AS_TEXT,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onOpenFileAsText));
    Connect(CMD_FILE_EXPLORER_RENAME_FILE,          wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onRenameFile));
    Connect(CMD_FILE_EXPLORER_DELETE_FILE,          wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorer::onDeleteFile));

    // create image list
    _iconList = new wxImageList(16, 16);
    _iconIndices[ICON_REFRESH]           = _iconList->Add(wxBitmap(wxT("system/developer/icons/refresh.png"),           wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_NEW_DIRECTORY]     = _iconList->Add(wxBitmap(wxT("system/developer/icons/directory-new.png"),     wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_NEW_FILE]          = _iconList->Add(wxBitmap(wxT("system/developer/icons/file-new.png"),          wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_DELETE]            = _iconList->Add(wxBitmap(wxT("system/developer/icons/delete.png"),            wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_DIRECTORY_CLOSED]  = _iconList->Add(wxBitmap(wxT("system/developer/icons/directory-closed.png"),  wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_DIRECTORY_OPEN]    = _iconList->Add(wxBitmap(wxT("system/developer/icons/directory-open.png"),    wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_FILE_TYPE_UNKNOWN] = _iconList->Add(wxBitmap(wxT("system/developer/icons/file-type-unknown.png"), wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_FILE_TYPE_PROJECT] = _iconList->Add(wxBitmap(wxT("system/developer/icons/file-type-project.png"), wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_FILE_TYPE_SCRIPT]  = _iconList->Add(wxBitmap(wxT("system/developer/icons/file-type-script.png"),  wxBITMAP_TYPE_PNG));
    _iconIndices[ICON_FILE_TYPE_TEXT]    = _iconList->Add(wxBitmap(wxT("system/developer/icons/file-type-text.png"),    wxBITMAP_TYPE_PNG));
    SetImageList(_iconList);

    if (_rootPath != wxEmptyString) {
        initFileTree(rootPath);
    }
}

//-------------------------------------------------------------------
FileExplorer::~FileExplorer()
{
    if (_iconList) {
        delete _iconList;
    }
}

//-------------------------------------------------------------------
const wxString&
FileExplorer::getRootPath() const
{
    return _rootPath;
}

//-------------------------------------------------------------------
void
FileExplorer::clearFileTree()
{
    DeleteAllItems();
}

//-------------------------------------------------------------------
void
FileExplorer::initFileTree(const wxString& rootPath)
{
    if (!wxDir::Exists(rootPath)) {
        return;
    }

    // set root path
    _rootPath = rootPath;

    // clear tree
    DeleteAllItems();

    // initialize tree
    wxTreeItemId root = AddRoot(wxFileName(rootPath).GetFullName(), -1, -1, new TreeItemData(FT_DIRECTORY));
    setItemIcons(root);
    buildItemTree(root);

    // expand root
    Expand(root);
}

//-------------------------------------------------------------------
void
FileExplorer::fileSaved(const wxString& fileName)
{
    if (fileName.Find(_rootPath) == 0) {
        wxFileName    path = fileName.Mid(_rootPath.Length());
        wxString      name = path.GetFullName();
        wxArrayString dirs = path.GetDirs();

        // find the last subdirectory
        wxTreeItemId lastSubDirItem = GetRootItem();
        for (size_t i = 0; i < dirs.GetCount(); i++) {
            bool nextSubDirFound = false;
            wxString nextSubDirName = dirs.Item(i);

            wxTreeItemIdValue cookie;
            wxTreeItemId child = GetFirstChild(lastSubDirItem, cookie);

            while (!nextSubDirFound && child.IsOk()) {
                wxString childName = GetItemText(child);

                if (childName == nextSubDirName) {
                    TreeItemData* childData = (TreeItemData*)GetItemData(child);

                    if (childData->Type == FT_DIRECTORY) { // found next directory
                        nextSubDirFound = true;
                        lastSubDirItem = child;
                    }
                }

                if (!nextSubDirFound) { // continue iterating over the children
                    child = GetNextChild(lastSubDirItem, cookie);
                }
            }

            if (!nextSubDirFound) { // next directory doesn't exist, just use buildItemTree() and return
                buildItemTree(child);
                return;
            }
        }

        // at this point all subdirectories exist and lastSubDirItem points to the last subdirectory
        // see if the file already has an item
        bool fileFound = false;

        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(lastSubDirItem, cookie);

        while (!fileFound && child.IsOk()) {
            wxString childName = GetItemText(child);

            if (childName == name) {
                TreeItemData* childData = (TreeItemData*)GetItemData(child);

                if (childData->Type != FT_DIRECTORY) { // found file
                    fileFound = true;
                }
            }

            if (!fileFound) { // continue iterating over the children
                child = GetNextChild(lastSubDirItem, cookie);
            }
        }

        if (!fileFound) {
            // add the file
            int fileType = _developer->getFileType(fileName);
            wxTreeItemId fileItem = AppendItem(lastSubDirItem, name, -1, -1, new TreeItemData(fileType));
            setItemIcons(fileItem);
        }
    }
}

//-------------------------------------------------------------------
bool
FileExplorer::removeAll(const wxString& dirName)
{
    { // this block is needed, so that the directory is closed before removing it
        wxDir dir(dirName);
        if (dir.IsOpened()) {
            wxString entryName;
            bool entryFound = dir.GetFirst(&entryName);

            // walk through the directory contents and remove them
            while (entryFound) {
                wxFileName entryFileName(dir.GetName() + wxFileName::GetPathSeparator() + entryName);

                if (wxDir::Exists(entryFileName.GetFullPath())) {
                    // entry is a subdirectory
                    if (!removeAll(entryFileName.GetFullPath())) {
                        return false;
                    }
                } else {
                    // entry is a regular file
                    if (!wxRemoveFile(entryFileName.GetFullPath())) {
                        return false;
                    }
                }

                entryFound = dir.GetNext(&entryName);
            }
        }
    }

    // directory is now empty, time to remove it
    return wxDir::Remove(dirName);
}

//-------------------------------------------------------------------
wxString
FileExplorer::constructPathFromItem(wxTreeItemId item)
{
    if (item == GetRootItem()) {
        return _rootPath;
    }

    wxString     path   = GetItemText(item);
    wxTreeItemId parent = GetItemParent(item);

    while (parent != GetRootItem()) {
        path   = GetItemText(parent) + wxFileName::GetPathSeparator() + path;
        parent = GetItemParent(parent);
    }

    path = _rootPath + wxFileName::GetPathSeparator() + path;

    return path;
}

//-------------------------------------------------------------------
void
FileExplorer::buildItemTree(wxTreeItemId item)
{
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);

    // it it's not a directory, nothing to build
    if (itemData->Type != FT_DIRECTORY) {
        return;
    }

    // if the directory item has children, delete them
    if (ItemHasChildren(item)) {
        DeleteChildren(item);
    }

    // enumerate all files in the directory and add them as children
    wxDir dir(constructPathFromItem(item));
    if (dir.IsOpened()) {
        wxString entryName;
        bool entryFound = dir.GetFirst(&entryName);

        while (entryFound) {
            wxString entryFileName = dir.GetName() + wxFileName::GetPathSeparator() + entryName;
            int entryFileType = _developer->getFileType(entryFileName);
            wxTreeItemId entryItem = AppendItem(item, entryName, -1, -1, new TreeItemData(entryFileType));
            setItemIcons(entryItem);

            if (entryFileType == FT_DIRECTORY) {
                buildItemTree(entryItem); // recursively build all sub-directories
            }

            entryFound = dir.GetNext(&entryName);
        }
    }
}

//-------------------------------------------------------------------
bool
FileExplorer::isItemChildOf(wxTreeItemId item1, wxTreeItemId item2)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId item = GetFirstChild(item2, cookie);

    while (item.IsOk()) {
        if (item == item1) {
            return true;
        }

        if (ItemHasChildren(item)) {
            if (isItemChildOf(item1, item)) {
                return true;
            }
        }

        item = GetNextChild(item2, cookie);
    }

    return false;
}

//-------------------------------------------------------------------
void
FileExplorer::setItemIcons(wxTreeItemId item)
{
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);
    switch (itemData->Type) {
    case FT_DIRECTORY:
        SetItemImage(item, ICON_DIRECTORY_CLOSED, wxTreeItemIcon_Normal);
        SetItemImage(item, ICON_DIRECTORY_OPEN,   wxTreeItemIcon_Expanded);
        break;
    case FT_PROJECT:
        SetItemImage(item, ICON_FILE_TYPE_PROJECT, wxTreeItemIcon_Normal);
        break;
    case FT_SCRIPT:
        SetItemImage(item, ICON_FILE_TYPE_SCRIPT, wxTreeItemIcon_Normal);
        break;
    case FT_TEXT:
        SetItemImage(item, ICON_FILE_TYPE_TEXT, wxTreeItemIcon_Normal);
        break;
    default:
        SetItemImage(item, ICON_FILE_TYPE_UNKNOWN, wxTreeItemIcon_Normal);
    }
}

//-------------------------------------------------------------------
wxMenu*
FileExplorer::createItemContextMenu(wxTreeItemId item)
{
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);

    wxMenu*     menu     = 0;
    wxMenu*     subMenu  = 0;
    wxMenuItem* menuItem = 0;

    menu = new wxMenu();

    if (item == GetRootItem()) {
        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_REFRESH, _("Refresh"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/refresh.png"), wxBITMAP_TYPE_PNG));
        menu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_EXPAND, _("Expand"));
        menu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_COLLAPSE, _("Collapse"));
        menu->Append(menuItem);

        menu->AppendSeparator();

        subMenu = new wxMenu();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_CREATE_NEW_DIRECTORY, _("New Directory"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/directory-new.png"), wxBITMAP_TYPE_PNG));
        subMenu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_CREATE_NEW_FILE, _("New File"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-add-new.png"), wxBITMAP_TYPE_PNG));
        subMenu->Append(menuItem);

        menu->AppendSubMenu(subMenu, _("Add"));

    } else if (itemData->Type == FT_DIRECTORY) {
        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_REFRESH, _("Refresh"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/refresh.png"), wxBITMAP_TYPE_PNG));
        menu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_EXPAND, _("Expand"));
        menu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_COLLAPSE, _("Collapse"));
        menu->Append(menuItem);

        menu->AppendSeparator();

        subMenu = new wxMenu();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_CREATE_NEW_DIRECTORY, _("New Directory"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/directory-new.png"), wxBITMAP_TYPE_PNG));
        subMenu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_CREATE_NEW_FILE, _("New File"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-add-new.png"), wxBITMAP_TYPE_PNG));
        subMenu->Append(menuItem);

        menu->AppendSubMenu(subMenu, _("Add"));

        menu->AppendSeparator();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_RENAME_FILE, _("Rename"));
        menu->Append(menuItem);

        menu->AppendSeparator();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_DELETE_FILE, _("Delete"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/delete.png"), wxBITMAP_TYPE_PNG));
        menu->Append(menuItem);

    } else {
        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_OPEN_FILE, _("Open"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-open.png"), wxBITMAP_TYPE_PNG));
        menu->Append(menuItem);

        subMenu = new wxMenu();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_OPEN_FILE_AS_SCRIPT, _("Script"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-type-script.png"), wxBITMAP_TYPE_PNG));
        subMenu->Append(menuItem);

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_OPEN_FILE_AS_TEXT, _("Text"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/file-type-text.png"), wxBITMAP_TYPE_PNG));
        subMenu->Append(menuItem);

        menu->AppendSubMenu(subMenu, _("Open As"));

        menu->AppendSeparator();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_RENAME_FILE, _("Rename"));
        menu->Append(menuItem);

        menu->AppendSeparator();

        menuItem = new wxMenuItem(0, CMD_FILE_EXPLORER_DELETE_FILE, _("Delete"));
        menuItem->SetBitmap(wxBitmap(wxT("system/developer/icons/delete.png"), wxBITMAP_TYPE_PNG));
        menu->Append(menuItem);
    }

    return menu;
}

//-------------------------------------------------------------------
void
FileExplorer::onItemBeginLabelEdit(wxTreeEvent& event)
{
    if (event.GetItem() == GetRootItem()) { // root item can't be edited
        event.Veto();
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onItemEndLabelEdit(wxTreeEvent& event)
{
    wxTreeItemId  item        = event.GetItem();
    TreeItemData* itemData    = (TreeItemData*)GetItemData(item);
    wxString      newItemName = event.GetLabel();

    wxFileName oldItemPath(constructPathFromItem(item));
    wxFileName newItemPath(oldItemPath.GetPathWithSep() + newItemName);

    if (!newItemName.IsEmpty() && newItemName != oldItemPath.GetFullName()) {
        wxString forbiddenChars = wxFileName::GetForbiddenChars();
        for (size_t i = 0; i < forbiddenChars.Length(); i++) {
            if (newItemName.Find(forbiddenChars.GetChar(i)) != wxNOT_FOUND) {
                wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
                ed.SetMessage(wxT("File name cannot contain any of the following characters: ") + forbiddenChars);
                ed.ShowModal();

                event.Veto();
                return;
            }
        }

        if (wxRenameFile(oldItemPath.GetFullPath(), newItemPath.GetFullPath())) {
            itemData->Type = _developer->getFileType(newItemPath.GetFullPath());
            setItemIcons(item); // update item icons, since the rename could have resulted in a diferent file type
            return;
        }
    }

    event.Veto();
}

//-------------------------------------------------------------------
void
FileExplorer::onItemBeginDrag(wxTreeEvent& event)
{
    if (event.GetItem() != GetRootItem()) { // root item can't be dragged
        event.Allow();
        _draggedItem = event.GetItem();
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onItemEndDrag(wxTreeEvent& event)
{
    wxTreeItemId  draggedItem     = _draggedItem;
    int           draggedItemType = ((TreeItemData*)GetItemData(draggedItem))->Type;
    wxString      draggedItemPath = constructPathFromItem(draggedItem);
    wxString      draggedItemName = GetItemText(draggedItem);

    wxTreeItemId parentItem = event.GetItem();
    if (((TreeItemData*)GetItemData(parentItem))->Type != FT_DIRECTORY) {
        parentItem = GetItemParent(parentItem);
    }

    // make sure the parent item is not a child item of the dragged item
    if (isItemChildOf(parentItem, draggedItem)) {
        wxMessageDialog ed(this, wxEmptyString, wxT("Developer"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        ed.SetMessage(wxT("Could not move file '") + draggedItemName + wxT("'. Invalid destination directory."));
        ed.ShowModal();
        return;
    }

    // construct the new file path
    wxString draggedItemNewPath = constructPathFromItem(parentItem) + wxFileName::GetPathSeparator() + draggedItemName;

    // make sure that moving the file will have an effect
    if (draggedItemPath == draggedItemNewPath) {
        return;
    }

    // move file
    if (wxRenameFile(draggedItemPath, draggedItemNewPath)) {
        wxTreeItemId newItem = AppendItem(parentItem, draggedItemName, -1, -1, new TreeItemData(draggedItemType));
        setItemIcons(newItem);
        Delete(draggedItem);

        if (draggedItemType == FT_DIRECTORY) {
            buildItemTree(newItem);
        }

        Expand(parentItem);
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onItemRightClick(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    SelectItem(item); // always select on right click

    // show context menu
    wxMenu* contextMenu = createItemContextMenu(item);
    PopupMenu(contextMenu);
    delete contextMenu;
}

//-------------------------------------------------------------------
void
FileExplorer::onItemActivated(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);
    if (itemData->Type == FT_DIRECTORY) {
        event.Skip(); // skip this event, so that the directory will be collapsed/expanded
        return;
    }

    _developer->openFile(constructPathFromItem(event.GetItem()));
}

//-------------------------------------------------------------------
void
FileExplorer::onRefreshTree(wxCommandEvent& event)
{
    buildItemTree(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onExpandTree(wxCommandEvent& event)
{
    ExpandAllChildren(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onCollapseTree(wxCommandEvent& event)
{
    CollapseAllChildren(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onAddNewDirectory(wxCommandEvent& event)
{
    wxTreeItemId  parentItem     = GetFocusedItem();
    wxString      parentItemPath = constructPathFromItem(parentItem);

    NewDirectoryDialog ndd;
    if (ndd.ShowModal() == wxID_OK) {
        wxString newDirName = ndd.getDirectoryName();
        wxString newDirPath = parentItemPath + wxFileName::GetPathSeparator() + newDirName;

        if (wxDir::Exists(newDirPath)) {
            wxMessageDialog ed(this, wxEmptyString, wxT("New Directory"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            ed.SetMessage(wxT("A directory named '") + newDirName + wxT("' already exists."));
            ed.ShowModal();
            return;
        }

        if (wxDir::Make(newDirPath)) {
            wxTreeItemId newDirItem = AppendItem(parentItem, newDirName, -1, -1, new TreeItemData(FT_DIRECTORY));
            setItemIcons(newDirItem);
            Expand(newDirItem);
        }
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onAddNewFile(wxCommandEvent& event)
{
    wxTreeItemId  parentItem     = GetFocusedItem();
    wxString      parentItemPath = constructPathFromItem(parentItem);

    _developer->addNewFile(parentItemPath);
}

//-------------------------------------------------------------------
void
FileExplorer::onOpenFile(wxCommandEvent& event)
{
    _developer->openFile(constructPathFromItem(GetFocusedItem()));
}

//-------------------------------------------------------------------
void
FileExplorer::onOpenFileAsScript(wxCommandEvent& event)
{
    _developer->openFileAs(constructPathFromItem(GetFocusedItem()), FT_SCRIPT);
}

//-------------------------------------------------------------------
void
FileExplorer::onOpenFileAsText(wxCommandEvent& event)
{
    _developer->openFileAs(constructPathFromItem(GetFocusedItem()), FT_TEXT);
}

//-------------------------------------------------------------------
void
FileExplorer::onRenameFile(wxCommandEvent& event)
{
    EditLabel(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onDeleteFile(wxCommandEvent& event)
{
    bool deleteSucceeded = false;

    wxTreeItemId  item     = GetFocusedItem();
    int           itemType = ((TreeItemData*)GetItemData(item))->Type;
    wxString      itemPath = constructPathFromItem(item);


    wxMessageDialog confirmDialog(this, wxEmptyString, wxT("Developer"), wxOK | wxCANCEL | wxCENTRE | wxCANCEL_DEFAULT | wxICON_QUESTION);

    if (itemType == FT_DIRECTORY) {
        confirmDialog.SetMessage(wxT("Do you want to delete the directory '") + GetItemText(item) + wxT("' and all its content?"));
        if (confirmDialog.ShowModal() == wxID_OK && removeAll(itemPath)) {
            deleteSucceeded = true;
        }
    } else {
        confirmDialog.SetMessage(wxT("Do you want to delete the file '") + GetItemText(item) + wxT("'?"));
        if (confirmDialog.ShowModal() == wxID_OK && wxRemoveFile(itemPath)) {
            deleteSucceeded = true;
        }
    }

    if (deleteSucceeded) {
        Delete(item);
    }
}
