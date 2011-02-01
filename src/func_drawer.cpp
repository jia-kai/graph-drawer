/*
 * $File: func_drawer.cpp
 * $Date: Tue Feb 01 00:06:06 2011 +0800
 *
 * implementation of FuncDrawer class
 *
 */

#include "func_drawer.h"

#include <gtkmm/container.h>
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

void FuncDrawer::draw(const Rectangle &domain, int width, int height)
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

	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, false, 8, width, height);

	FillImageProgressReporter x;
	m_func.fill_image(pixbuf->get_pixels(), width, height,
			m_prev_domain, x);
	this->set(pixbuf);
}

void FuncDrawer::redraw()
{
	Gtk::Container *par = this->get_parent();
	if (par)
	{
		Gtk::Allocation allocation = par->get_allocation();
		draw(m_prev_domain, allocation.get_width(), allocation.get_height());
	}
}


