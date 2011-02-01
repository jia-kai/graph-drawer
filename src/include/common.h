/*
 * $File: common.h
 * $Date: Mon Jan 31 23:36:58 2011 +0800
 *
 * some common definitions and constants
 */

#ifndef _HEADER_COMMON_
#define _HEADER_COMMON_

typedef double Real_t;
static const Real_t EPS = 1e-6;

class Rectangle
{
	public:
		Real_t left, bottom, right, top;
		Rectangle(Real_t x0 = 0, Real_t y0 = 0, Real_t x1 = 0, Real_t y1 = 0);
		bool operator == (const Rectangle &n) const;
};


#endif
