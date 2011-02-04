/*
 * $File: mandelbrot.cpp
 * $Date: Fri Feb 04 22:09:55 2011 +0800
 *
 * compute the Mandelbrot set
 *
 */

#include "mandelbrot.h"
#include <cstdlib>

Rectangle Function_mandelbrot::get_initial_domain() const
{
	return Rectangle(-2, -2, 2, 2);
}

void Function_mandelbrot::fill_image(uint8_t *buf, int width, int height,
		const Rectangle &domain, FillImageProgressReporter &progress_reporter) const
{
	const int LIMIT = 100000;
	Real_t delta_x = (domain.right - domain.left) / width,
		   delta_y = (domain.top - domain.bottom) / height,
		   y0 = domain.bottom,
		   progress = 0,
		   progress_delta = 1.0 / width / height;
	for (int i = 0; i < height; i ++)
	{
		Real_t x0 = domain.left;
		for (int j = 0; j < width; j ++)
		{
			Real_t cx = x0, cy = y0;
			x0 += delta_x;

			int cnt = LIMIT;
			for (Real_t x = 0, y = 0, x_sqr, y_sqr;
					(x_sqr = x * x) + (y_sqr = y * y) <= 4 && (-- cnt); )
			{
				y = 2 * x * y + cy;
				x = x_sqr - y_sqr + cx;
			}

			int color = (int)(256.0 * 256 * 256 * cnt / LIMIT);
			*(buf ++) = color >> 16;
			*(buf ++) = (color >> 8) & 0xFF;
			*(buf ++) = color & 0xFF;

			progress_reporter.report(progress += progress_delta);
		}
		y0 += delta_y;
	}
}

