/*
 * $File: func_drawer.cpp
 * $Date: Tue Feb 01 16:45:14 2011 +0800
 *
 * implementation of FuncDrawer class
 *
 */

#include "func_drawer.h"

#include <cmath>

class FillImageProgressReporter : public Function::FillImageProgressReporter
{
	void report(double percentage)
	{
	}
};

FuncDrawer::FuncDrawer(const Function &func) :
	m_func(func), m_prev_domain(func.get_initial_domain()),
	m_prev_width(-1), m_prev_height(-1)
{
}

FuncDrawer::~FuncDrawer()
{
}

void FuncDrawer::gen_pixbuf(const Rectangle &domain, int width, int height)
{
	if (width == m_prev_width && height == m_prev_height && domain == m_prev_domain)
		return;
	m_prev_width = width;
	m_prev_height = height;

	Real_t x0 = domain.left, y0 = domain.bottom,
		   x1 = domain.right, y1 = domain.top;

	if (fabs((x1 - x0) / (y1 - y0) - Real_t(width) / height) > EPS)
	{
		Real_t r1 = (x1 - x0) / (y1 - y0),
			   r2 = Real_t(width) / height;
		if (r1 > r2)
			x1 = x0 + (y1 - y0) * r2;
		else
			y1 = y0 + (x1 - x0) / r2;
	}

	m_prev_domain = Rectangle(x0, y0, x1, y1);

	m_p_pixbuf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, false, 8, width, height);

	FillImageProgressReporter x;
	m_func.fill_image(m_p_pixbuf->get_pixels(), width, height,
			m_prev_domain, x);
}

bool FuncDrawer::on_expose_event(GdkEventExpose *event)
{
	Glib::RefPtr<Gdk::Window> win = this->get_window();
	if (win)
	{
		Gtk::Allocation allocation = this->get_allocation();
		gen_pixbuf(m_prev_domain, allocation.get_width(), allocation.get_height());
		win->draw_pixbuf(Gdk::GC::create(win),
				m_p_pixbuf,
				event->area.x, event->area.y,	// src x y
				event->area.x, event->area.y,	// dest x y
				event->area.width, event->area.height,
				Gdk::RGB_DITHER_NONE, 0, 0);
	}
	return true;
}

