/*
 * $File: func_drawer.cpp
 * $Date: Fri Feb 04 12:45:53 2011 +0800
 *
 * implementation of FuncDrawer class
 *
 */

#include "func_drawer.h"

#include <cairomm/context.h>
#include <cmath>


class FuncDrawer::RenderProgressBar : public Function::FillImageProgressReporter
{
	public:
		RenderProgressBar(FuncDrawer *drawer);

		void report(double progress);
		void redraw(int x, int y, int width, int height);	// called with 0, 0, 0, 0 to redraw the whole area
		void set_bg_pixbuf(Glib::RefPtr<Gdk::Pixbuf> &bg_pixbuf);
		void reset_progress();
	
	private:

		// target FuncDrawer to be painted on
		FuncDrawer *m_p_func_drawer;

		// background pixbuf
		Glib::RefPtr<Gdk::Pixbuf> m_p_bg_pixbuf;

		// current progress, ranging in [0, 1]
		double m_progress;

		Glib::Mutex m_mutex;

		static const double
			BAR_WIDTH	= 0.8,	// relative to window width
			BAR_HEIGHT	= 30,	// absolute value in pixels
			FONT_SIZE	= 20,
			PROGRESS_DELTA = 0.01;	// progress bar will be updated after making at least such progress
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
			printf("render_pixbuf: rendering, return false\n");
			m_mutex.unlock();
			return false;
		}
	} else
		m_mutex.unlock();

	// now the rendering thread can not be running, so locking is not needed
	if (!(width == m_prev_width && height == m_prev_height && domain == m_prev_domain))
	{
		m_p_rpbar->reset_progress();
		m_p_rpbar->redraw(0, 0, 0, 0);
		m_render_thread_exit_flag = false;
		m_cur_render_width = width;
		m_cur_render_height = height;
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

	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, false, 8, width, height);

	m_func.fill_image(pixbuf->get_pixels(), width, height,
			new_domain, *m_p_rpbar);


	{
		Glib::Mutex::Lock _lock_(m_mutex);
		m_p_pixbuf = pixbuf;
		m_prev_width = width;
		m_prev_height = height;
		m_prev_domain = Rectangle(x0, y0, x1, y1);
		if (!m_render_thread_exit_flag)
		{
			printf("render_pixbuf_do: ready to draw\n");
			draw_pixbuf(0, 0, width, height);
			m_p_rpbar->set_bg_pixbuf(pixbuf);
			printf("render_pixbuf_do: finished drawing\n");
			m_p_render_thread = NULL;
		}
	}
}

void FuncDrawer::draw_pixbuf(int x, int y, int width, int height)
{
	printf("draw_pixbuf %d %d %d %d\n", x, y, width, height);
	Glib::RefPtr<Gdk::Window> win = this->get_window();
	if (win && m_p_pixbuf)
	{
		win->draw_pixbuf(Gdk::GC::create(win),
				m_p_pixbuf,
				x, y,	// src x y
				x, y,	// dest x y
				width, height,
				Gdk::RGB_DITHER_NONE, 0, 0);
	}
	printf("draw_pixbuf done\n");
}

bool FuncDrawer::on_expose_event(GdkEventExpose *event)
{
	if (event)
	{
		printf("on_expose_event\n");
		m_mutex.lock();
		if (m_p_render_thread)
		{
			m_mutex.unlock();
			m_p_rpbar->redraw(event->area.x, event->area.y,
					event->area.width, event->area.height);
		} else
		{
			m_mutex.unlock();
			Gtk::Allocation allocation = this->get_allocation();
			if (render_pixbuf(m_prev_domain, allocation.get_width(), allocation.get_height()))
			{
				draw_pixbuf(event->area.x, event->area.y,
						event->area.width, event->area.height);
			}
		}
	}
	return true;
}

FuncDrawer::RenderProgressBar::RenderProgressBar(FuncDrawer *drawer) :
	m_p_func_drawer(drawer), m_p_bg_pixbuf(NULL)
{
}

void FuncDrawer::RenderProgressBar::set_bg_pixbuf(Glib::RefPtr<Gdk::Pixbuf> &bg_pixbuf)
{
	m_p_bg_pixbuf = bg_pixbuf;
}

void FuncDrawer::RenderProgressBar::reset_progress()
{
	m_progress = 0;
}

void FuncDrawer::RenderProgressBar::report(double progress)
{
	bool update = false;
	{
		Glib::Mutex::Lock _lock_(m_mutex);
		if (m_p_func_drawer->m_render_thread_exit_flag)
			throw Glib::Thread::Exit();
		if (progress - m_progress > PROGRESS_DELTA || 1 - progress < EPS)
		{
			update = true;
			m_progress = progress;
		}
	}
	if (update)
		redraw(0, 0, 0, 0);
}

void FuncDrawer::RenderProgressBar::redraw(int x, int y, int width, int height)
{
	{
		Glib::Mutex::Lock _lock_(m_mutex);
		Glib::RefPtr<Gdk::Window> window = m_p_func_drawer->get_window();
		if (window)
		{
			Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
			if (width && height)
			{
				cr->rectangle(x, y, width, height);
				cr->clip();
			}

			if (m_p_bg_pixbuf)
			{
				Gdk::Cairo::set_source_pixbuf(cr, m_p_bg_pixbuf, 0, 0);
				cr->paint();
			}

			Gtk::Allocation allocation = m_p_func_drawer->get_allocation();
			width = allocation.get_width();
			height = allocation.get_height();


			// background rectangle
			cr->set_line_width(2);
			cr->set_source_rgba(1, 1, 1, 0.5);
			cr->rectangle(width * (1.0 - BAR_WIDTH) * 0.5,
					(height - BAR_HEIGHT) * 0.5, width * BAR_WIDTH, BAR_HEIGHT);
			cr->fill();


			// progress rectangle
			cr->set_source_rgba(0.4, 0.4, 0.6, 0.6);
			cr->rectangle(width * (1.0 - BAR_WIDTH) * 0.5,
					(height - BAR_HEIGHT) * 0.5, width * BAR_WIDTH * m_progress, BAR_HEIGHT);
			cr->fill();


			// text
			cr->set_source_rgba(0, 0, 0, 0.8);
			cr->set_font_size(FONT_SIZE);
			cr->select_font_face("Monospace", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
			char rpbar_msg[256];
			sprintf(rpbar_msg, "Rendering %.2lf%% ...", m_progress * 100);
			Cairo::TextExtents rmsg_extents;
			cr->get_text_extents(rpbar_msg, rmsg_extents);
			cr->move_to(width * 0.5 - (rmsg_extents.x_bearing + rmsg_extents.x_advance) * 0.5,
					height * 0.5 - (rmsg_extents.y_bearing + rmsg_extents.y_advance) * 0.5);
			cr->show_text(rpbar_msg);
			cr->stroke();
		}
	}
}

