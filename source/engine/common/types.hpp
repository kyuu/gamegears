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

#ifndef SPHERE_TYPES_HPP
#define SPHERE_TYPES_HPP


namespace sphere {

#ifdef _MSC_VER
    typedef signed   __int8  i8;
    typedef signed   __int16 i16;
    typedef signed   __int32 i32;
    typedef signed   __int64 i64;
    typedef unsigned __int8  u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;
#else
#include <stdint.h>
    typedef int8_t   i8;
    typedef int16_t  i16;
    typedef int32_t  i32;
    typedef int64_t  i64;
    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
#endif

    typedef float  f32;
    typedef double f64;

    typedef unsigned int uint;

}


#endif
