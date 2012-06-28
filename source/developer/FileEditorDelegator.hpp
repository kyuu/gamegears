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

#ifndef FILEEDITORDELEGATOR_HPP
#define FILEEDITORDELEGATOR_HPP

#include "IFileEditorDelegator.hpp"


template<typename fileEditorT>
class FileEditorDelegator : public IFileEditorDelegator {
public:
    explicit FileEditorDelegator(fileEditorT* documentEditor) : _fileEditor(documentEditor) { }
    ~FileEditorDelegator() { }

    // IFileEditorDelegator implementation
    int getFileEditorType() const {
        return _fileEditor->getFileEditorType();
    }

    const wxString& getFileName() const {
        return _fileEditor->getFileName();
    }

    bool isFileModified() const {
        return _fileEditor->isFileModified();
    }

    bool saveFile() {
        return _fileEditor->saveFile();
    }

    bool saveFileAs(const wxString& fileName = wxEmptyString) {
        return _fileEditor->saveFileAs(fileName);
    }

    bool canDoCommand(int cmd) const {
        return _fileEditor->canDoCommand(cmd);
    }

    void doCommand(int cmd) {
        _fileEditor->doCommand(cmd);
    }

private:
    fileEditorT* _fileEditor;
};


#endif
