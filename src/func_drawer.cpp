/*
 * $File: func_drawer.cpp
 * $Date: Wed Feb 02 23:42:24 2011 +0800
 *
 * implementation of FuncDrawer class
 *
 */

#include "func_drawer.h"

#include <cairomm/context.h>
#include <cmath>


#define LOCK Glib::Mutex::Lock __this_name_shoud_not_be_used_it_is_a_mutex_lock__(this->m_mutex)

class FuncDrawer::RenderProgressBar : public Function::FillImageProgressReporter
{
	public:
		RenderProgressBar(Gtk::DrawingArea *drawing_area);

		void report(double progress);
		void redraw(int x, int y, int width, int height);
		void set_bg_pixbuf(Glib::RefPtr<Gdk::Pixbuf> &bg_pixbuf);
	
	private:

		// target DrawingArea to be painted on
		Gtk::DrawingArea *m_p_drawing_area;

		// background pixbuf
		Glib::RefPtr<Gdk::Pixbuf> m_p_bg_pixbuf;

		// current progress, in [0, 1]
		double m_progress;
};

FuncDrawer::FuncDrawer(const Function &func) :
	m_func(func), m_prev_domain(func.get_initial_domain()),
	m_prev_width(-1), m_prev_height(-1),
	m_p_render_thread(NULL),
	m_p_rpbar(new RenderProgressBar(this))
{
}

FuncDrawer::~FuncDrawer()
{
	delete m_p_rpbar;
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

	m_func.fill_image(pixbuf->get_pixels(), width, height,
			new_domain, *m_p_rpbar);


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
		/*
		win->draw_pixbuf(Gdk::GC::create(win),
				m_p_pixbuf,
				event->area.x, event->area.y,	// src x y
				event->area.x, event->area.y,	// dest x y
				event->area.width, event->area.height,
				Gdk::RGB_DITHER_NONE, 0, 0);
				*/
		m_p_rpbar->set_bg_pixbuf(m_p_pixbuf);
		m_p_rpbar->redraw(
				event->area.x, event->area.y,
				event->area.width, event->area.height);
	}
	return true;
}

FuncDrawer::RenderProgressBar::RenderProgressBar(Gtk::DrawingArea *drawing_area) :
	m_p_drawing_area(drawing_area), m_p_bg_pixbuf(NULL)
{
}

void FuncDrawer::RenderProgressBar::set_bg_pixbuf(Glib::RefPtr<Gdk::Pixbuf> &bg_pixbuf)
{
	m_p_bg_pixbuf = bg_pixbuf;
}

void FuncDrawer::RenderProgressBar::report(double progress)
{
}

void FuncDrawer::RenderProgressBar::redraw(int x, int y, int width, int height)
{
	Glib::RefPtr<Gdk::Window> window = m_p_drawing_area->get_window();
	if (window)
	{
		Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
		cr->rectangle(x, y, width, height);
		cr->clip();

		if (m_p_bg_pixbuf)
		{
			Gdk::Cairo::set_source_pixbuf(cr, m_p_bg_pixbuf, 0, 0);
			cr->paint();
		}

	}
}

