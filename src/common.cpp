/*
 * $File: common.cpp
 * $Date: Mon Jan 31 23:38:09 2011 +0800
 */

#include "common.h"
#include <algorithm>
#include <cmath>
using namespace std;

Rectangle::Rectangle(Real_t x0, Real_t y0, Real_t x1, Real_t y1)
{
	if (x0 > x1)
		swap(x0, x1);
	if (y0 > y1)
		swap(y0, y1);
	left = x0;
	right = x1;
	bottom = y0;
	top = y1;
}

bool Rectangle::operator == (const Rectangle &n) const
{
	return
		fabs(left - n.left) < EPS &&
		fabs(right - n.right) < EPS &&
		fabs(top - n.top) < EPS &&
		fabs(bottom - n.bottom) < EPS;
}

