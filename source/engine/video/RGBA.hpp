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

#ifndef SPHERE_RGBA_HPP
#define SPHERE_RGBA_HPP

#include "../common/types.hpp"


namespace sphere {

    struct RGBA {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;

        RGBA() : red(0), green(0), blue(0), alpha(255) { }
        RGBA(u8 r, u8 g, u8 b, u8 a = 255) : red(r), green(g), blue(b), alpha(a) { }
        RGBA(const RGBA& that) : red(that.red), green(that.green), blue(that.blue), alpha(that.alpha) { }

        bool operator==(const RGBA& rhs) {
            return (red   == rhs.red   &&
                    green == rhs.green &&
                    blue  == rhs.blue  &&
                    alpha == rhs.alpha);
        }

        bool operator!=(const RGBA& rhs) {
            return !(*this == rhs);
        }

        static u32 Pack(u8 red, u8 green, u8 blue, u8 alpha = 255) {
            return ((red << 24) | (green << 16) | (blue << 8) | alpha);
        }

        static u32 Pack(const RGBA& that) {
            return Pack(that.red, that.green, that.blue, that.alpha);
        }

        static RGBA Unpack(u32 packed) {
            return RGBA(packed >> 24, packed >> 16, packed >> 8, packed);
        }
    };

} // namespace sphere


#endif
