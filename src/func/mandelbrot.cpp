/*
 * $File: mandelbrot.cpp
 * $Date: Fri Feb 04 12:44:24 2011 +0800
 *
 * compute the Mandelbrot set
 *
 */

#include "mandelbrot.h"
#include <cstdlib>

Rectangle Function_mandelbrot::get_initial_domain() const
{
	return Rectangle(0, 0, 1, 1);
}

void Function_mandelbrot::fill_image(uint8_t *buf, int width, int height,
		const Rectangle &domain, FillImageProgressReporter &progress_reporter) const
{
	for (int i = 0; i < height; i ++)
		for (int j = 0; j < width; j ++)
			for (int k = 0; k < 3; k ++)
			{
				*(buf ++) = (uint8_t)(rand() / (RAND_MAX + 1.0) * 256);
				progress_reporter.report(double((i * width + j) * 3 + k) / (width * height * 3.0));
			}
}

