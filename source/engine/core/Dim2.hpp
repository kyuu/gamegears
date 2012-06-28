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

#ifndef SPHERE_DIM2_HPP
#define SPHERE_DIM2_HPP

#include "../common/types.hpp"


namespace sphere {

    template<typename T>
    struct Dim2 {
        T width;
        T height;

        Dim2() : width(0), height(0) { }
        template<typename U>
        Dim2(U w, U h) : width(w), height(h) { }
        Dim2(const Dim2& that) : width(that.width), height(that.height) { }

        Dim2& operator=(const Dim2& that) {
            width  = that.width;
            height = that.height;
            return *this;
        }

        T getArea() const {
            return width * height;
        }

    };

    typedef Dim2<u32> Dim2u;
    typedef Dim2<i32> Dim2i;
    typedef Dim2<f32> Dim2f;

} // namespace sphere


#endif
