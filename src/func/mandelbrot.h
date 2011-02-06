/*
 * $File: mandelbrot.h
 * $Date: Sun Feb 06 12:03:52 2011 +0800
 *
 * compute the Mandelbrot set
 *
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

#ifndef _HEADER_MANDELBROT_
#define _HEADER_MANDELBROT_

#include "function.h"

class Function_mandelbrot : public Function
{
	public:
		Function_mandelbrot();
		Rectangle get_initial_domain();
		void fill_image(uint8_t *buf, int width, int height,
				const Rectangle &domain, FillImageProgressReporter &progress_reporter);

	private:
		void on_key_press(int keyval);
		int m_nthread, m_nloop;
};

#endif
