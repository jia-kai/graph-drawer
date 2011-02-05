/*
 * $File: common.h
 * $Date: Sat Feb 05 18:15:29 2011 +0800
 *
 * some common definitions and constants
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

