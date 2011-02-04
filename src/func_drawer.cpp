/*
 * $File: func_drawer.cpp
 * $Date: Fri Feb 04 22:56:24 2011 +0800
 *
 * implementation of FuncDrawer class
 *
 */

#include "func_drawer.h"

#include <cairomm/context.h>
#include <cmath>

#define LOCK Glib::Mutex::Lock _mutex_lock_var_(m_mutex)

static const double
	PROGRESS_BAR_DELTA	= 0.002,// the bar will be redrawed after making at lest such progress
	PROGRESS_BAR_WIDTH	= 0.8,	// relative width to the width of the window
	PROGRESS_BAR_HEIGHT	= 30,	// height in pixels
	PROGRESS_BAR_BORDER	= 5,	// border width in pixels
	PROGRESS_BAR_FONT_SIZE	= 25;

FuncDrawer::FuncDrawer(const Function &func) :
	m_func(func), m_prev_domain(func.get_initial_domain()),
	m_prev_width(-1), m_prev_height(-1),
	m_p_render_thread(NULL)
{
	m_sig_progress.connect(sigc::bind(
				sigc::mem_fun(*this, &FuncDrawer::draw_rpbar),
				0, 0, 0, 0));
	m_sig_render_done.connect(sigc::bind(
				sigc::mem_fun(*this, &FuncDrawer::draw_pixbuf),
				0, 0, 0, 0));
}

FuncDrawer::~FuncDrawer()
{
}

bool FuncDrawer::render_pixbuf(const Rectangle &domain, int width, int height)
{
	m_mutex.lock();
	if (m_p_render_thread)
	{
		if (width != m_cur_render_width || height != m_cur_render_height)
		{
			m_render_thread_exit_flag = true;
			m_mutex.unlock();
			m_p_render_thread->join();
			m_prev_width = -1; // make sure it will be re-rendered
		} else
		{
			m_mutex.unlock();
			return false;
		}
	} else
		m_mutex.unlock();

	// now the rendering thread can not be running, so locking is not needed
	if (!(width == m_prev_width && height == m_prev_height && domain == m_prev_domain))
	{
		m_render_thread_exit_flag = false;
		m_cur_render_width = width;
		m_cur_render_height = height;
		m_render_progress = 0;
		m_p_render_thread = Glib::Thread::create(
				sigc::bind(sigc::mem_fun(*this, &FuncDrawer::render_pixbuf_do),
					domain, width, height), true);
		return false;
	}

	return true;
}

void FuncDrawer::render_pixbuf_do(const Rectangle &domain, int width, int height)
{
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
	g_assert(fabs((x1 - x0) / (y1 - y0) - Real_t(width) / height) <= EPS);

	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, false, 8, width, height);
	pixbuf->fill(0);
	{
		LOCK;
		m_p_pixbuf = pixbuf;
	}

	m_func.fill_image(pixbuf->get_pixels(), width, height,
			new_domain, *this);


	{
		LOCK;
		if (!m_render_thread_exit_flag)
		{
			m_prev_width = width;
			m_prev_height = height;
			m_prev_domain = Rectangle(x0, y0, x1, y1);
			m_sig_render_done.emit();
			m_p_render_thread = NULL;
		}
	}
}

void FuncDrawer::report(double progress)
{
	{
		LOCK;
		if (progress - m_render_progress > PROGRESS_BAR_DELTA)
		{
			m_render_progress = progress;
			m_sig_progress.emit();
		}
	}
}

bool FuncDrawer::test_abort()
{
	{
		LOCK;
		return m_render_thread_exit_flag;
	}
}

void FuncDrawer::draw_pixbuf(int x, int y, int width, int height)
{
	{
		LOCK;
		Glib::RefPtr<Gdk::Window> win = this->get_window();
		if (win && m_p_pixbuf)
		{
			if (!width || !height)
			{
				x = y = 0;
				width = height = -1;
			}
			win->draw_pixbuf(Gdk::GC::create(win),
					m_p_pixbuf,
					x, y,	// src x y
					x, y,	// dest x y
					width, height,
					Gdk::RGB_DITHER_NONE, 0, 0);
		}
	}
}

bool FuncDrawer::on_expose_event(GdkEventExpose *event)
{
	if (event)
	{
		Gtk::Allocation allocation = this->get_allocation();
		if (render_pixbuf(m_prev_domain, allocation.get_width(), allocation.get_height()))
		{
			draw_pixbuf(event->area.x, event->area.y,
					event->area.width, event->area.height);
		} else
		{
			draw_rpbar(event->area.x, event->area.y,
					event->area.width, event->area.height);
		}
	}
	return true;
}

void FuncDrawer::draw_rpbar(int x, int y, int width, int height)
{
	{
		LOCK;
		Glib::RefPtr<Gdk::Window> win = this->get_window();
		if (win)
		{
			Cairo::RefPtr<Cairo::Context> cr = win->create_cairo_context();
			if (width && height)
			{
				cr->rectangle(x, y, width, height);
				cr->clip();
			}

			Gtk::Allocation allocation = this->get_allocation();
			width = allocation.get_width();
			height = allocation.get_height();

			if (m_p_pixbuf)
			{
				Gdk::Cairo::set_source_pixbuf(cr, m_p_pixbuf, 0, 0);
				cr->paint();
			} else
			{
				cr->set_source_rgb(0, 0, 0);
				cr->rectangle(0, 0, width, height);
				cr->fill();
			}

			double bar_width = width * PROGRESS_BAR_WIDTH,
				   bar_height = PROGRESS_BAR_HEIGHT;

			// background rectangle
			cr->set_line_width(1);
			cr->set_source_rgba(1, 1, 1, 0.5);
			cr->rectangle(
					(width - bar_width) * 0.5,
					(height - bar_height) * 0.5,
					bar_width,
					bar_height);
			cr->fill();


			// progress rectangle
			bar_width -= PROGRESS_BAR_BORDER;
			bar_height -= PROGRESS_BAR_BORDER;
			cr->set_source_rgba(0.6, 0.2, 0.1, 0.7);
			cr->rectangle(
					(width - bar_width) * 0.5,
					(height - bar_height) * 0.5,
					bar_width * m_render_progress,
					bar_height);
			cr->fill();


			// text
			cr->set_source_rgba(0, 0, 0, 0.8);
			cr->set_font_size(PROGRESS_BAR_FONT_SIZE);
			cr->select_font_face("Monospace", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
			char rpbar_msg[256];
			sprintf(rpbar_msg, "Rendering %.2lf%% ...", m_render_progress * 100);
			Cairo::TextExtents rmsg_extents;
			cr->get_text_extents(rpbar_msg, rmsg_extents);
			cr->move_to(width * 0.5 - (rmsg_extents.x_bearing + rmsg_extents.x_advance) * 0.5,
					height * 0.5 - (rmsg_extents.y_bearing + rmsg_extents.y_advance) * 0.5);
			cr->show_text(rpbar_msg);
			cr->stroke();
		}
	}
}

