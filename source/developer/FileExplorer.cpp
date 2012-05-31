#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/treebase.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "Developer.hpp"
#include "FileExplorer.hpp"

#include <iostream>
#include <fstream>
extern std::ofstream Log;


//-------------------------------------------------------------------
struct TreeItemData : public wxTreeItemData {
    TreeItemData(const wxString& path, int type) : Path(path), Type(type) { }
    TreeItemData(const TreeItemData& that) : Path(that.Path), Type(that.Type) { }
    ~TreeItemData() { }

    TreeItemData& operator=(const TreeItemData& that) {
        Path = that.Path;
        Type = that.Type;
        return *this;
    }

    int Type;
    wxString Path;
};

//-------------------------------------------------------------------
FileExplorer::FileExplorer(Developer* developer, const std::string& rootName, const std::string& rootPath)
    : wxTreeCtrl(developer, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS)
    , _developer(developer)
    , _rootPath(rootPath)
    , _draggedItem(0)
    , _imageList(0)
{
    // connect events
    Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(FileExplorer::onItemBeginLabelEdit));
    Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT,   wxTreeEventHandler(FileExplorer::onItemEndLabelEdit));
    Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG,       wxTreeEventHandler(FileExplorer::onItemBeginDrag));
    Connect(wxEVT_COMMAND_TREE_END_DRAG,         wxTreeEventHandler(FileExplorer::onItemEndDrag));
    Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(FileExplorer::onItemRightClick));
    Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED ,  wxTreeEventHandler(FileExplorer::onItemActivated));

    Connect(ID_REFRESH_TREE,        wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onRefreshTree);
    Connect(ID_EXPAND_TREE,         wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onExpandTree);
    Connect(ID_COLLAPSE_TREE,       wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onCollapseTree);
    Connect(ID_ADD_DIRECTORY,       wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onAddDirectory);
    Connect(ID_OPEN_FILE,           wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onOpenFile);
    Connect(ID_OPEN_FILE_AS_SCRIPT, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onOpenFileAsScript);
    Connect(ID_OPEN_FILE_AS_TEXT,   wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onOpenFileAsText);
    Connect(ID_EDIT_ITEM,           wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onEditItem);
    Connect(ID_DELETE_ITEM,         wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileExplorer::onDeleteItem);

    // create image list
    _imageList = new wxImageList(16, 16);
        idProjectRoot  = _imageList->Add(wxBitmap(wxT("system/project-root.png"),  wxBITMAP_TYPE_PNG));
        idProjectFile  = _imageList->Add(wxBitmap(wxT("system/project-file.png"),  wxBITMAP_TYPE_PNG));
        idFolderClosed = _imageList->Add(wxBitmap(wxT("system/folder-closed.png"), wxBITMAP_TYPE_PNG));
        idFolderOpen   = _imageList->Add(wxBitmap(wxT("system/folder-open.png"),   wxBITMAP_TYPE_PNG));
        idScriptFile   = _imageList->Add(wxBitmap(wxT("system/script-file.png"),   wxBITMAP_TYPE_PNG));
        idTextFile     = _imageList->Add(wxBitmap(wxT("system/text-file.png"),     wxBITMAP_TYPE_PNG));
        idImageFile    = _imageList->Add(wxBitmap(wxT("system/image-file.png"),    wxBITMAP_TYPE_PNG));
        idAudioFile    = _imageList->Add(wxBitmap(wxT("system/audio-file.png"),    wxBITMAP_TYPE_PNG));
        idUnknownFile  = _imageList->Add(wxBitmap(wxT("system/unknown-file.png"),  wxBITMAP_TYPE_PNG));
    SetImageList(_imageList);

    wxTreeItemId root = AddRoot(rootName, idProjectRoot, -1, new TreeItemData(rootPath, true));
    buildItemTree(root);
    Expand(root);
}

//-------------------------------------------------------------------
FileExplorer::~FileExplorer()
{
    if (_imageList) {
        delete _imageList;
    }
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
    wxDir dir(itemData->Path);
    if (dir.IsOpened()) {
        wxString entryName;
        bool entryFound = dir.GetFirst(&entryName);

        while (entryFound) {
            wxString entryFileName = dir.GetName() + wxFileName::GetPathSeparator() + entryName;
            int entryFileType = getFileType(entryFileName);
            wxTreeItemId entryItem = AppendItem(item, entryName, -1, -1, new TreeItemData(entryFileName, entryFileType));
            setItemIcons(entryItem);

            if (entryFileType == FT_DIRECTORY) {
                buildItemTree(entryItem); // recursively update all sub-directories
            }

            entryFound = dir.GetNext(&entryName);
        }
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onItemBeginLabelEdit(wxTreeEvent& e)
{
    if (e.GetItem() == GetRootItem()) { // root item can't be edited
        e.Veto();
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onItemEndLabelEdit(wxTreeEvent& e)
{
    wxString newName = e.GetLabel();
    wxFileName oldPath(((TreeItemData*)GetItemData(e.GetItem()))->Path);

    if (!newName.IsEmpty() && newName != oldPath.GetFullName()) {
        // see if the new name contains any path separator, we don't want them here
        if (newName.Find(':')  != wxNOT_FOUND ||
            newName.Find('\\') != wxNOT_FOUND ||
            newName.Find('/')  != wxNOT_FOUND)
        {
            wxMessageDialog errorDialog(this, wxEmptyString, wxT("Error"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            errorDialog.SetMessage(wxT("Can not rename '") + oldPath.GetFullName() + wxT("' to '") + newName + wxT("'. The new name contains invalid characters."));
            errorDialog.ShowModal();
        } else {
            if (wxRenameFile(oldPath.GetFullPath(), oldPath.GetPathWithSep() + newName)) {
                buildItemTree(GetItemParent(e.GetItem()));
                return;
            }
        }
    }
    e.Veto();
}

//-------------------------------------------------------------------
void
FileExplorer::onItemBeginDrag(wxTreeEvent& e)
{
    if (e.GetItem() != GetRootItem()) { // root item can't be dragged
        e.Allow();
        _draggedItem = e.GetItem();
    }
}

//-------------------------------------------------------------------
int
FileExplorer::getFileType(const wxString& filename)
{
    // first see if it's a directory
    if (wxDir::Exists(filename)) {
        return FT_DIRECTORY;
    }

    // now look at the file extension
    std::string ext = wxFileName(filename).GetExt().ToStdString();

    // see if it's a project file
    std::vector<std::string>& projectFileTypes = _developer->getPreferences().FileExplorer.FileTypes.Project;
    for (size_t i = 0; i < projectFileTypes.size(); i++) {
        if (ext == projectFileTypes[i]) {
            return FT_PROJECT;
        }
    }

    // see if it's a script file
    std::vector<std::string>& scriptFileTypes = _developer->getPreferences().FileExplorer.FileTypes.Script;
    for (size_t i = 0; i < scriptFileTypes.size(); i++) {
        if (ext == scriptFileTypes[i]) {
            return FT_SCRIPT;
        }
    }

    // see if it's a text file
    std::vector<std::string>& textFileTypes = _developer->getPreferences().FileExplorer.FileTypes.Text;
    for (size_t i = 0; i < textFileTypes.size(); i++) {
        if (ext == textFileTypes[i]) {
            return FT_TEXT;
        }
    }

    // see if it's a image file
    std::vector<std::string>& imageFileTypes = _developer->getPreferences().FileExplorer.FileTypes.Image;
    for (size_t i = 0; i < imageFileTypes.size(); i++) {
        if (ext == imageFileTypes[i]) {
            return FT_IMAGE;
        }
    }

    // see if it's a audio file
    std::vector<std::string>& audioFileTypes = _developer->getPreferences().FileExplorer.FileTypes.Audio;
    for (size_t i = 0; i < audioFileTypes.size(); i++) {
        if (ext == audioFileTypes[i]) {
            return FT_AUDIO;
        }
    }

    return FT_UNKNOWN;
}

//-------------------------------------------------------------------
void
FileExplorer::setItemIcons(wxTreeItemId item)
{
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);
    switch (itemData->Type) {
    case FT_DIRECTORY:
        SetItemImage(item, idFolderClosed, wxTreeItemIcon_Normal);
        SetItemImage(item, idFolderOpen,   wxTreeItemIcon_Expanded);
        break;
    case FT_PROJECT:
        SetItemImage(item, idProjectFile, wxTreeItemIcon_Normal);
        break;
    case FT_SCRIPT:
        SetItemImage(item, idScriptFile, wxTreeItemIcon_Normal);
        break;
    case FT_TEXT:
        SetItemImage(item, idTextFile, wxTreeItemIcon_Normal);
        break;
    case FT_IMAGE:
        SetItemImage(item, idImageFile, wxTreeItemIcon_Normal);
        break;
    case FT_AUDIO:
        SetItemImage(item, idAudioFile, wxTreeItemIcon_Normal);
        break;
    default:
        SetItemImage(item, idUnknownFile, wxTreeItemIcon_Normal);
    }
}

//-------------------------------------------------------------------
bool
FileExplorer::isItemChildOf(wxTreeItemId possibleChild, wxTreeItemId possibleParent)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId search;
    wxTreeItemId item = GetFirstChild(possibleParent, cookie);
    wxTreeItemId child;

    while (item.IsOk()) {
        if (item == possibleChild) {
            return true;
        }

        if (ItemHasChildren(item)) {
            bool result = isItemChildOf(possibleChild, item);
            if (result) {
                return true;
            }
        }

        item = GetNextChild(possibleParent, cookie);
    }

    return false;
}

//-------------------------------------------------------------------
void
FileExplorer::onItemEndDrag(wxTreeEvent& e)
{
    wxTreeItemId draggedItem = _draggedItem;
    _draggedItem = 0;
    TreeItemData* draggedItemData = (TreeItemData*)GetItemData(draggedItem);
    wxFileName draggedItemPath(draggedItemData->Path);
    wxString draggedItemName = draggedItemPath.GetFullName();
    int draggedItemType = draggedItemData->Type;

    wxTreeItemId parentItem = e.GetItem();
    TreeItemData* parentItemData = (TreeItemData*)GetItemData(parentItem);
    if (parentItemData->Type != FT_DIRECTORY) {
        parentItem = GetItemParent(parentItem);
        parentItemData = (TreeItemData*)GetItemData(parentItem);
    }

    // make sure the parent item is not really a child item of the dragged item
    if (isItemChildOf(parentItem, draggedItem)) {
        wxMessageDialog errorDialog(this, wxEmptyString, wxT("Error"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
        errorDialog.SetMessage(wxT("Can not move '") + draggedItemName + wxT("'. Invalid destination directory."));
        errorDialog.ShowModal();
        return;
    }

    wxString draggedItemNewPath = parentItemData->Path + wxFileName::GetPathSeparator() + draggedItemName;
    if (wxRenameFile(draggedItemPath.GetFullPath(), draggedItemNewPath)) {
        wxTreeItemId newItem = AppendItem(parentItem, draggedItemName, -1, -1, new TreeItemData(draggedItemNewPath, draggedItemType));
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
FileExplorer::onItemRightClick(wxTreeEvent& e)
{
    wxTreeItemId item = e.GetItem();
    SelectItem(item); // always select on right click
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);

    wxMenu menu;
    if (item == GetRootItem()) {
        menu.Append(ID_REFRESH_TREE,  wxT("Refresh Tree"));
        menu.Append(ID_EXPAND_TREE,   wxT("Expand Tree"));
        menu.Append(ID_COLLAPSE_TREE, wxT("Collapse Tree"));
        menu.AppendSeparator();
        menu.Append(ID_ADD_DIRECTORY, wxT("Add Directory"));
    } else if (itemData->Type == FT_DIRECTORY) {
        menu.Append(ID_REFRESH_TREE,  wxT("Refresh Tree"));
        menu.Append(ID_EXPAND_TREE,   wxT("Expand Tree"));
        menu.Append(ID_COLLAPSE_TREE, wxT("Collapse Tree"));
        menu.AppendSeparator();
        menu.Append(ID_ADD_DIRECTORY, wxT("Add Directory"));
        menu.AppendSeparator();
        menu.Append(ID_EDIT_ITEM,     wxT("Rename"));
        menu.AppendSeparator();
        menu.Append(ID_DELETE_ITEM,   wxT("Delete"));
    } else {
        menu.Append(ID_OPEN_FILE,     wxT("Open"));
        wxMenu* openAsSubMenu = new wxMenu();
            openAsSubMenu->Append(ID_OPEN_FILE_AS_SCRIPT, _("&Script"));
            openAsSubMenu->Append(ID_OPEN_FILE_AS_TEXT, _("&Text"));
        menu.AppendSubMenu(openAsSubMenu, _("Open &As..."));
        menu.AppendSeparator();
        menu.Append(ID_EDIT_ITEM,   wxT("Rename"));
        menu.AppendSeparator();
        menu.Append(ID_DELETE_ITEM, wxT("Delete"));
    }
    PopupMenu(&menu);
}

//-------------------------------------------------------------------
void
FileExplorer::onItemActivated(wxTreeEvent& e)
{
    wxTreeItemId item = e.GetItem();
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);
    if (itemData->Type == FT_DIRECTORY) {
        e.Skip();
    }
    // TODO
}

//-------------------------------------------------------------------
void
FileExplorer::onRefreshTree(wxCommandEvent& e)
{
    buildItemTree(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onExpandTree(wxCommandEvent& e)
{
    ExpandAllChildren(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onCollapseTree(wxCommandEvent& e)
{
    CollapseAllChildren(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onAddDirectory(wxCommandEvent& e)
{
    wxTreeItemId parentItem = GetFocusedItem();
    TreeItemData* parentItemData = (TreeItemData*)GetItemData(parentItem);

    wxTextEntryDialog enterDirNameDialog(this, wxT("Enter the name of the new directory."));
    int confirmed = enterDirNameDialog.ShowModal() == wxID_OK;
    if (confirmed) {
        wxString newDirName = enterDirNameDialog.GetValue();

        if (newDirName.IsEmpty()) {
            wxMessageDialog errorDialog(this, wxEmptyString, wxT("Error"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            errorDialog.SetMessage(wxT("Can not add directory. The directory name is empty."));
            errorDialog.ShowModal();
            return;
        }

        wxFileName parentPath(parentItemData->Path);
        wxFileName newDirPath(parentPath.GetFullPath() + wxFileName::GetPathSeparator() + newDirName);

        if (wxDir::Exists(newDirPath.GetFullPath())) {
            wxMessageDialog errorDialog(this, wxEmptyString, wxT("Error"), wxOK | wxCENTRE | wxOK_DEFAULT | wxICON_ERROR);
            errorDialog.SetMessage(wxT("Can not add directory. A directory named '") + newDirName + wxT("' already exists."));
            errorDialog.ShowModal();
            return;
        }

        if (wxDir::Make(newDirPath.GetFullPath())) {
            wxTreeItemId newDirItem = AppendItem(parentItem, newDirName, -1, -1, new TreeItemData(newDirPath.GetFullPath(), FT_DIRECTORY));
            setItemIcons(newDirItem);
            Expand(newDirItem);
        }
    }
}

//-------------------------------------------------------------------
void
FileExplorer::onOpenFile(wxCommandEvent& e)
{
    // TODO
}

//-------------------------------------------------------------------
void
FileExplorer::onOpenFileAsScript(wxCommandEvent& e)
{
    // TODO
}

//-------------------------------------------------------------------
void
FileExplorer::onOpenFileAsText(wxCommandEvent& e)
{
    // TODO
}

//-------------------------------------------------------------------
void
FileExplorer::onEditItem(wxCommandEvent& e)
{
    EditLabel(GetFocusedItem());
}

//-------------------------------------------------------------------
void
FileExplorer::onDeleteItem(wxCommandEvent& e)
{
    wxTreeItemId item = GetFocusedItem();
    TreeItemData* itemData = (TreeItemData*)GetItemData(item);
    bool deleteSucceeded = false;

    wxMessageDialog confirmDialog(this, wxEmptyString, wxT("Confirmation Required"), wxOK | wxCANCEL | wxCENTRE | wxCANCEL_DEFAULT | wxICON_QUESTION);

    if (itemData->Type == FT_DIRECTORY) {
        confirmDialog.SetMessage(wxT("Are you sure you want to delete '") + GetItemText(item) + wxT("' and all its content?"));
        int confirmed = confirmDialog.ShowModal() == wxID_OK;
        if (confirmed && removeDirRecursive(itemData->Path)) {
            deleteSucceeded = true;
        }
    } else {
        confirmDialog.SetMessage(wxT("Are you sure you want to delete '") + GetItemText(item) + wxT("'?"));
        int confirmed = confirmDialog.ShowModal() == wxID_OK;
        if (confirmed && wxRemoveFile(itemData->Path)) {
            deleteSucceeded = true;
        }
    }

    if (deleteSucceeded) {
        Delete(item);
    }
}

//-------------------------------------------------------------------
bool
FileExplorer::removeDirRecursive(const wxString& dirName)
{
    { // this block is needed, because wxDir can only be closed in it's destructor
        wxDir dir(dirName);
        if (dir.IsOpened()) {
            wxString entryName;
            bool entryFound = dir.GetFirst(&entryName);

            // walk through the directory contents and delete them
            while (entryFound) {
                wxFileName entryFileName(dir.GetName() + wxFileName::GetPathSeparator() + entryName);

                if (wxDir::Exists(entryFileName.GetFullPath())) {
                    // entry is a subdirectory
                    if (!removeDirRecursive(entryFileName.GetFullPath())) {
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

    // directory is now empty, time to delete it
    return wxDir::Remove(dirName);
}
