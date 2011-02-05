/*
 * $File: common.cpp
 * $Date: Sat Feb 05 19:59:18 2011 +0800
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

#include "common.h"
#include <cmath>

Rectangle::Rectangle(Real_t x_, Real_t y_, Real_t width_, Real_t height_) :
	x(x_), y(y_), width(width_), height(height_)
{
}

bool Rectangle::operator == (const Rectangle &n) const
{
	return
		fabs(x - n.x) < EPS &&
		fabs(y - n.y) < EPS &&
		fabs(width - n.width) < EPS &&
		fabs(height - n.height) < EPS;
}

