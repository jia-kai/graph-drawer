/*
 * $File: common.h
 * $Date: Sat Feb 05 19:59:36 2011 +0800
 *
 * some common definitions and constants
 */
/*
	This file is part of graph-drawer, a gtkmm based function graph drawer

	Copyright (C) <2011>  Jiakai <jia.kai66@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _HEADER_COMMON_
#define _HEADER_COMMON_

typedef float Real_t;
static const Real_t EPS = 1e-6;

class Rectangle
{
	public:
		Real_t x, y, width, height;

		Rectangle(Real_t x_ = 0, Real_t y_ = 0, Real_t width_ = 0, Real_t height_ = 0);
		bool operator == (const Rectangle &n) const;
		Rectangle& operator = (const Rectangle &n)
		{ x = n.x; y = n.y; width = n.width; height = n.height; return *this; }
};


#endif

