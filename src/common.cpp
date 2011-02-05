/*
 * $File: common.cpp
 * $Date: Sat Feb 05 17:32:13 2011 +0800
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

