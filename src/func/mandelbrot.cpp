/*
 * $File: mandelbrot.cpp
 * $Date: Sun Feb 06 10:51:55 2011 +0800
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

#include "mandelbrot.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>

#include <unistd.h>
#include <pthread.h>

static const int
	NITER_DEFAULT	= 100000, //65536,
	NITER_PER_LOOP	= 1000, //NITER_DEFAULT, //512,
	USLEEP_TIME		= 500000;

struct Thread_param_t
{
	bool stop_flag;
	uint8_t *buf, *buf_end;
	Real_t
		*cx, *cy,	// corresponding value of C for each pixel
		*x, *y;		// current value after iteration

	// number of iterations already done for each pixel
	// niter[i] = -1 if it's sure to diverge
	int *niter;

	int pdelta,			// number of pixels to jump
		nloop;			// number of outer loops
	uint64_t cur_niter_done;

	Thread_param_t() :
		stop_flag(false), cur_niter_done(0)
	{}
};
static void* thread_render(void *param);
static int str2num(const char *str);

Function_mandelbrot::Function_mandelbrot()
{
	printf("Mandelbrot set: usable enviroment variables:\n"
			"  threads   -- the number of rendering threads\n"
			"  iteration -- the number of maximal iterations\n");
	m_nthread = str2num(getenv("threads"));
	if (m_nthread <= 0)
		m_nthread = this->get_cpu_num();
	int niter = str2num(getenv("iteration"));
	if (niter <= 0)
		niter = NITER_DEFAULT;
	m_nloop = (niter - 1) / NITER_PER_LOOP + 1;

	printf("Mandelbrot set: use %d threads and %d iterations\n",
			m_nthread, m_nloop * NITER_PER_LOOP);
}

Rectangle Function_mandelbrot::get_initial_domain() const
{
	return Rectangle(-2, -2, 4, 4);
}

void Function_mandelbrot::fill_image(uint8_t *buf, int width, int height,
		const Rectangle &domain, FillImageProgressReporter &progress_reporter) const
{
	Thread_param_t *params = new Thread_param_t[m_nthread];
	pthread_t *threads = new pthread_t[m_nthread];

	int img_size = width * height;
	Real_t *cx = new Real_t[img_size],
		   *cy = new Real_t[img_size],
		   *x = new Real_t[img_size],
		   *y = new Real_t[img_size];
	memset(x, 0, sizeof(Real_t) * img_size);
	memset(y, 0, sizeof(Real_t) * img_size);
	for (int i = 0, p = 0; i < height; i ++)
	{
		Real_t cy_val = domain.y + domain.height * i / height;
		for (int j = 0; j < width; j ++, p ++)
		{
			cx[p] = domain.x + domain.width * j / width;
			cy[p] = cy_val;
		}
	}
	int *niter = new int[img_size];
	memset(niter, 0, sizeof(int) * img_size);
	for (int i = 0; i < m_nthread; i ++)
	{
		params[i].buf = buf + i * 3;
		params[i].buf_end = buf + img_size * 3;
		params[i].cx = cx + i;
		params[i].cy = cy + i;
		params[i].x = x + i;
		params[i].y = y + i;
		params[i].niter = niter + i;
		params[i].pdelta = m_nthread;
		params[i].nloop = m_nloop;
		pthread_create(threads + i, NULL, thread_render, params + i);
	}

	uint64_t niter_tot = width * height;
	niter_tot *= NITER_PER_LOOP;
	niter_tot *= m_nloop;
	for (; ;)
	{
		if (progress_reporter.test_abort())
		{
			for (int i = 0; i < m_nthread; i ++)
			{
				params[i].stop_flag = true;
				pthread_join(threads[i], NULL);
			}
			break;
		}
		bool done = true;
		uint64_t niter_done = 0;
		for (int i = 0; i < m_nthread; i ++)
		{
			niter_done += params[i].cur_niter_done;
			if (!params[i].stop_flag)
				done = false;
		}
		progress_reporter.report(double(niter_done) / niter_tot);
		if (done)
			break;
		usleep(USLEEP_TIME);
	}

	delete params;
	delete threads;
	delete cx;
	delete cy;
	delete x;
	delete y;
	delete niter;
}

int str2num(const char *str)
{
	if (!str)
		return -1;
	int ret = 0;
	while (*str)
	{
		if (!isdigit(*str))
			return -1;
		ret = ret * 10 + *(str ++) - '0';
	}
	return ret;
}

void* thread_render(void *_param)
{
	Thread_param_t *paramptr = static_cast<Thread_param_t*>(_param),
				   param = *paramptr;
	for (int i = 0; i < param.nloop; i ++)
	{
		Real_t *cxptr = param.cx, *cyptr = param.cy,
			   *xptr = param.x, *yptr = param.y;
		int *niterptr = param.niter;

		for (uint8_t *buf = param.buf; buf < param.buf_end;
				buf += param.pdelta * 3,
				cxptr += param.pdelta, cyptr += param.pdelta,
				xptr += param.pdelta, yptr += param.pdelta,
				niterptr += param.pdelta)
		{
			if (paramptr->stop_flag)
				return NULL;
			if (*niterptr == -1)
				continue;
			Real_t x = *xptr, y = *yptr;
			int niter = NITER_PER_LOOP;
			for (Real_t x_sqr, y_sqr, cx = *cxptr, cy = *cyptr;
					(x_sqr = x * x) + (y_sqr = y * y) <= 4 && niter; niter --)
			{
				y = 2 * x * y + cy;
				x = x_sqr - y_sqr + cx;
			}

			niter = NITER_PER_LOOP - niter;
			*niterptr += niter;
			paramptr->cur_niter_done += niter;
			if (x * x + y * y > 4 || (i == param.nloop - 1 && !niter))
			{
				int color = (int)(256 * 256 * 256 * (1.0 - double(*niterptr) / NITER_PER_LOOP / param.nloop));
				buf[0] = color >> 16;
				buf[1] = color & 0xFF;
				buf[2] = (color >> 8) & 0xFF;
				*niterptr = -1;
				paramptr->cur_niter_done += (param.nloop - i - 1) * NITER_PER_LOOP + NITER_PER_LOOP - niter;
			}

			*xptr = x;
			*yptr = y;
		}
	}

	paramptr->stop_flag = true;
	return NULL;
}

