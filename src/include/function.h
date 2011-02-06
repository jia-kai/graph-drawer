/*
 * $File: function.h
 * $Date: Sat Feb 05 22:42:06 2011 +0800
 *
 * base function class
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

#ifndef _HEADER_FUNCTION_
#define _HEADER_FUNCTION_

#include "common.h"
#include <stdint.h>

class Function
{
	public:
		virtual ~Function() {}

		virtual Rectangle get_initial_domain() const = 0;

		class FillImageProgressReporter
		{
			public:
				virtual void report(double progress) = 0;
				// @progress should be in [0, 1]

				virtual bool test_abort() = 0;
				// the computation function should return
				// as soon as possible if it finds test_abort() returns true
		};
		virtual void fill_image(uint8_t *buf, int width, int height,
				const Rectangle &domain, FillImageProgressReporter &progress_reporter) const = 0;
		// it's guaranteed that the @width/@height ratio is the same as that of @domain when this function get called
		// progress_reporter.report must be called after finishing each pixel

		int get_cpu_num();
		// get the number of CPUs currently available
};

#endif
