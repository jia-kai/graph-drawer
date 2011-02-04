/*
 * $File: function.h
 * $Date: Thu Feb 03 21:53:14 2011 +0800
 *
 * base function class
 */

#ifndef _HEADER_FUNCTION_
#define _HEADER_FUNCTION_

#include "common.h"
#include <stdint.h>

class Function
{
	public:
		virtual Rectangle get_initial_domain() const = 0;

		class FillImageProgressReporter
		{
			public:
				virtual void report(double progress) = 0;
				// @progress should be in [0, 1]
		};
		virtual void fill_image(uint8_t *buf, int width, int height,
				const Rectangle &domain, FillImageProgressReporter &progress_reporter) const = 0;
		// it's guaranteed that the @width/@height ratio is the same as that of @domain when this function get called
		// progress_reporter.report must be called after finishing each pixel
};

#endif
