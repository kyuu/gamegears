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

#ifndef IFILEEDITORDELEGATOR_HPP
#define IFILEEDITORDELEGATOR_HPP


class IFileEditorDelegator {
public:
    virtual int  getFileEditorType() const = 0;
    virtual const wxString& getFileName() const = 0;
    virtual bool isFileModified() const = 0;
    virtual bool saveFile() = 0;
    virtual bool saveFileAs(const wxString& fileName = wxEmptyString) = 0;
    virtual bool canDoCommand(int cmd) const = 0;
    virtual void doCommand(int cmd) = 0;

protected:
    virtual ~IFileEditorDelegator() { }
};


#endif
