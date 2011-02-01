/*
 * $File: mandelbrot.h
 * $Date: Mon Jan 31 22:40:07 2011 +0800
 *
 * compute the Mandelbrot set
 *
 */

#ifndef _HEADER_MANDELBROT_
#define _HEADER_MANDELBROT_

#include "function.h"

class Function_mandelbrot : public Function
{
	public:
		Rectangle get_initial_domain() const;
		void fill_image(uint8_t *buf, int width, int height,
				const Rectangle &domain, FillImageProgressReporter &progress_reporter) const;
};

#endif
