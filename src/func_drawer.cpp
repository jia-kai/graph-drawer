/*
 * $File: func_drawer.cpp
 * $Date: Wed Feb 02 21:00:18 2011 +0800
 *
 * implementation of FuncDrawer class
 *
 */

#include "func_drawer.h"

#include <cmath>
#define LOCK Glib::Mutex::Lock __this_name_shoud_not_be_used_it_is_a_mutex_lock__(this->m_mutex)

class FillImageProgressReporter : public Function::FillImageProgressReporter
{
	void report(double percentage)
	{
	}
};

FuncDrawer::FuncDrawer(const Function &func) :
	m_func(func), m_prev_domain(func.get_initial_domain()),
	m_prev_width(-1), m_prev_height(-1),
	m_p_render_thread(NULL)
{
}

FuncDrawer::~FuncDrawer()
{
}

void FuncDrawer::render_pixbuf(const Rectangle &domain, int width, int height)
{
	{
		LOCK;
		if (!m_p_render_thread)
		{
			m_p_render_thread = Glib::Thread::create(
					sigc::bind(sigc::mem_fun(*this, &FuncDrawer::render_pixbuf_do),
						domain, width, height), true);
		}
	}
}

void FuncDrawer::render_pixbuf_do(const Rectangle &domain, int width, int height)
{
	if (width == m_prev_width && height == m_prev_height && domain == m_prev_domain)
		return;

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
	Rectangle new_domain(x0, y0, x1, y1);

	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, false, 8, width, height);

	FillImageProgressReporter x; // XXX
	m_func.fill_image(pixbuf->get_pixels(), width, height,
			new_domain, x);


	{
		LOCK;
		m_p_pixbuf = pixbuf;
		m_prev_width = width;
		m_prev_height = height;
		m_prev_domain = Rectangle(x0, y0, x1, y1);
	}
}

bool FuncDrawer::on_expose_event(GdkEventExpose *event)
{
	Glib::RefPtr<Gdk::Window> win = this->get_window();
	if (win)
	{
		Gtk::Allocation allocation = this->get_allocation();
		render_pixbuf(m_prev_domain, allocation.get_width(), allocation.get_height());
		if (m_p_render_thread)
		{
			m_p_render_thread->join();
			m_p_render_thread = NULL;
		}
		win->draw_pixbuf(Gdk::GC::create(win),
				m_p_pixbuf,
				event->area.x, event->area.y,	// src x y
				event->area.x, event->area.y,	// dest x y
				event->area.width, event->area.height,
				Gdk::RGB_DITHER_NONE, 0, 0);
	}
	return true;
}

