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

#ifndef SPHERE_REFIMPL_HPP
#define SPHERE_REFIMPL_HPP


namespace sphere {

    template<class T>
    class RefImpl : public T {
    public:
        virtual void grab() {
            ++_count;
        }

        virtual void drop() {
            if (--_count == 0) {
                delete this;
            }
        }

    protected:
        RefImpl() : _count(1) { }
        virtual ~RefImpl() { }

    private:
        int _count;
    };

}


#endif
