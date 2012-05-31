#ifndef FILEEXPLORER_HPP
#define FILEEXPLORER_HPP

#include <string>
#include <wx/wx.h>
#include <wx/treectrl.h>


// forward declaration
class Developer;


class FileExplorer : public wxTreeCtrl {
public:
    enum {
        ID_REFRESH_TREE = 0,
        ID_EXPAND_TREE,
        ID_COLLAPSE_TREE,
        ID_ADD_DIRECTORY,
        ID_OPEN_FILE,
        ID_OPEN_FILE_AS_SCRIPT,
        ID_OPEN_FILE_AS_TEXT,
        ID_EDIT_ITEM,
        ID_DELETE_ITEM,
    };

    enum {
        FT_UNKNOWN = 0,
        FT_DIRECTORY,
        FT_PROJECT,
        FT_SCRIPT,
        FT_TEXT,
        FT_IMAGE,
        FT_AUDIO,
    };

    FileExplorer(Developer* developer, const std::string& rootName, const std::string& rootPath);
    ~FileExplorer();

    // event handlers
    void onItemBeginLabelEdit(wxTreeEvent& e);
    void onItemEndLabelEdit(wxTreeEvent& e);
    void onItemBeginDrag(wxTreeEvent& e);
    void onItemEndDrag(wxTreeEvent& e);
    void onItemRightClick(wxTreeEvent& e);
    void onItemActivated(wxTreeEvent& e);

    // context menu event handlers
    void onRefreshTree(wxCommandEvent& e);
    void onExpandTree(wxCommandEvent& e);
    void onCollapseTree(wxCommandEvent& e);
    void onAddDirectory(wxCommandEvent& e);
    void onOpenFile(wxCommandEvent& e);
    void onOpenFileAsScript(wxCommandEvent& e);
    void onOpenFileAsText(wxCommandEvent& e);
    void onEditItem(wxCommandEvent& e);
    void onDeleteItem(wxCommandEvent& e);

    // auxiliary method
    bool removeDirRecursive(const wxString& dirName);

private:
    int  getFileType(const wxString& ext);
    bool isItemChildOf(wxTreeItemId possibleChild, wxTreeItemId possibleParent);
    void buildItemTree(wxTreeItemId item);
    void setItemIcons(wxTreeItemId item);

private:
    Developer*  _developer;
    std::string _rootPath;

    // image indices into image list
    wxImageList* _imageList;
    int idProjectRoot;
    int idProjectFile;
    int idFolderClosed;
    int idFolderOpen;
    int idScriptFile;
    int idTextFile;
    int idImageFile;
    int idAudioFile;
    int idUnknownFile;

    // used in a drag event
    wxTreeItemId _draggedItem;
};


#endif
