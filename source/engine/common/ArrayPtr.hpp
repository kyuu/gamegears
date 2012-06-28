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

#ifndef SPHERE_ARRAYPTR_HPP
#define SPHERE_ARRAYPTR_HPP

#include <cassert>


namespace sphere {

    template<class T>
    class ArrayPtr {
    public:
        ArrayPtr(T* t = 0) : _t(t) {
        }

        ArrayPtr(const ArrayPtr<T>& that) : _t(that._t) {
            that._t = 0;
        }

        ~ArrayPtr() {
            if (_t) {
                delete[] _t;
            }
        }

        ArrayPtr<T>& operator=(T* t) {
            if (_t != t) {
                reset(t);
            }
            return *this;
        }

        ArrayPtr<T>& operator=(const ArrayPtr<T>& that) {
            if (this != &that) {
                reset(that._t);
                that._t = 0;
            }
            return *this;
        }

        bool operator==(const ArrayPtr<T>& that) {
            return _t == that._t;
        }

        bool operator!=(const ArrayPtr<T>& that) {
            return _t != that._t;
        }

        bool operator!() const {
            return _t == 0;
        }

        operator bool() const {
            return _t != 0;
        }

        T& operator[](size_t n) {
            assert(_t);
            return _t[n];
        }

        T* get() const {
            assert(_t);
            return _t;
        }

        T* release() {
            T* t = _t;
            _t = 0;
            return t;
        }

        void reset(T* t = 0) {
            if (_t) {
                delete[] _t;
            }
            _t = t;
        }

    private:
        T* _t;
        int _size;
    };

}


#endif
