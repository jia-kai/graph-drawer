/*
 * $File: func_drawer.cpp
 * $Date: Sun Feb 06 12:19:21 2011 +0800
 *
 * implementation of FuncDrawer class
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


/*
 * Implement FuncDrawer::Render_param_t
 */
FuncDrawer::Render_param_t::Render_param_t(const Rectangle &d, int w, int h):
	domain(d), width(w), height(h)
{
	adjust();
}

bool FuncDrawer::Render_param_t::operator == (const Render_param_t &n) const
{
	return width == n.width && height == n.height && domain == n.domain;
}

void FuncDrawer::Render_param_t::adjust()
{
	Real_t r1 = domain.width / domain.height,
		   r2 = Real_t(width) / height;
	if (fabs(r1 - r2) > EPS)
	{
		if (r1 > r2)
		{
			Real_t w1 = domain.height * r2;
			domain.x += (domain.width - w1) * 0.5;
			domain.width = w1;
		} else
		{
			Real_t h1 = domain.width / r2;
			domain.y += (domain.height - h1) * 0.5;
			domain.height = h1;
		}
	}
}


/*
 * Implement FuncDrawer
 */
FuncDrawer::FuncDrawer(Function &func) :
	m_func(func), m_func_domain(func.get_initial_domain()),
	m_p_render_thread(NULL)
{
	m_prev_rparam.width = -1;	// make sure the image will be rendered
	m_sig_progress.connect(sigc::bind(
				sigc::mem_fun(*this, &FuncDrawer::draw_rpbar),
				0, 0, 0, 0));
	m_sig_render_done.connect(sigc::bind(
				sigc::mem_fun(*this, &FuncDrawer::draw_pixbuf),
				0, 0, 0, 0));
}

FuncDrawer::~FuncDrawer()
{
	m_mutex.lock();
	if (m_p_render_thread)
	{
		m_render_thread_exit_flag = true;
		m_mutex.unlock();
		m_p_render_thread->join();
	} else m_mutex.unlock();
}

void FuncDrawer::report(double progress)
{
	if (progress > 1)
		progress = 1;
	{
		LOCK;
		if (m_render_thread_exit_flag)
			return;
		if (progress - m_render_progress > PROGRESS_BAR_DELTA)
		{
			m_render_progress = progress;
			m_sig_progress.emit();
		}
	}
}

const Rectangle& FuncDrawer::get_domain()
{
	return m_func_domain;
}

void FuncDrawer::set_domain(const Rectangle &domain)
{
	m_func_domain = domain;
	Gtk::Allocation allocation = this->get_allocation();
	render_pixbuf(Render_param_t(domain, allocation.get_width(), allocation.get_height()));
}

bool FuncDrawer::test_abort()
{
	{
		LOCK;
		return m_render_thread_exit_flag;
	}
}

void FuncDrawer::stop_render()
{
	m_mutex.lock();
	if (m_p_render_thread)
	{
		m_render_thread_exit_flag = true;
		m_mutex.unlock();
		m_p_render_thread->join();
	} else
		m_mutex.unlock();
}

bool FuncDrawer::render_pixbuf(const Render_param_t &param)
{
	m_mutex.lock();
	if (m_p_render_thread)
	{
		if (!(param == m_cur_rparam))
		{
			m_render_thread_exit_flag = true;
			m_mutex.unlock();
			m_p_render_thread->join();
			m_prev_rparam.width = -1; // make sure it will be re-rendered
		} else
		{
			m_mutex.unlock();
			return false;
		}
	} else
		m_mutex.unlock();

	// now the rendering thread can not be running, so locking is not needed
	if (param == m_prev_rparam)
		return true;
	m_render_thread_exit_flag = false;
	m_cur_rparam = param;
	m_render_progress = 0;
	m_p_render_thread = Glib::Thread::create(
			sigc::mem_fun(*this, &FuncDrawer::render_pixbuf_do), true);
	return false;
}

void FuncDrawer::render_pixbuf_do()
{
	Rectangle domain;
	int width, height;
	{
		LOCK;
		width = m_cur_rparam.width;
		height = m_cur_rparam.height;
		domain = m_cur_rparam.domain;
	}
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(
			Gdk::COLORSPACE_RGB, false, 8, width, height);
	pixbuf->fill(0);
	{
		LOCK;
		m_p_pixbuf = pixbuf;
	}

	m_func.fill_image(pixbuf->get_pixels(), width, height,
			domain, *this);


	{
		LOCK;
		m_prev_rparam = m_cur_rparam;
		m_sig_render_done.emit();
		m_p_render_thread = NULL;
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
		int width = allocation.get_width(), height = allocation.get_height();
		this->set_ratio((Real_t)width / height);
		if (render_pixbuf(Render_param_t(m_func_domain, allocation.get_width(), allocation.get_height())))
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

void FuncDrawer::save_to_bmp(const std::string &fpath)
{
	{
		LOCK;
		if (!m_p_render_thread)
			m_p_pixbuf->save(fpath, "bmp");
	}
}

void FuncDrawer::on_select(const Rectangle &area)
{
	{
		LOCK;
		if (m_p_render_thread)
			return;
	}
	Glib::RefPtr<Gdk::Drawable> win = this->get_window();
	if (win)
	{
		Gtk::Allocation allocation = this->get_allocation();
		double w = allocation.get_width(),
			   h = allocation.get_height();
		set_domain(Rectangle(
				m_func_domain.x + area.x / w * m_func_domain.width,
				m_func_domain.y + area.y / h * m_func_domain.height,
				m_func_domain.width * area.width / w,
				m_func_domain.height * area.height / h));
	}
}

void FuncDrawer::zoom(double factor)
{
	Rectangle d1;
	d1.width = m_func_domain.width * factor;
	d1.height = m_func_domain.height * factor;
	d1.x = m_func_domain.x - (d1.width - m_func_domain.width) * 0.5;
	d1.y = m_func_domain.y - (d1.height - m_func_domain.height) * 0.5;
	set_domain(d1);
}

bool FuncDrawer::on_motion_notify_event(GdkEventMotion *event)
{
	Gtk::Allocation allocation = this->get_allocation();
	double w = allocation.get_width(),
		   h = allocation.get_height();
	on_cursor_motion(
			m_func_domain.x + event->x / w * m_func_domain.width,
			m_func_domain.y + event->y / h * m_func_domain.height);
	return SelectionArea::on_motion_notify_event(event);
}

void FuncDrawer::on_right_button_press(int x, int y)
{
	Glib::RefPtr<Gdk::Drawable> win = this->get_window();
	if (win)
	{
		Gtk::Allocation allocation = this->get_allocation();
		double w = allocation.get_width(),
			   h = allocation.get_height();
		Rectangle d1(m_func_domain);
		d1.x += (x / w - 0.5) * d1.width;
		d1.y += (y / h - 0.5) * d1.height;
		set_domain(d1);
	}
}

void FuncDrawer::on_cursor_motion(Real_t, Real_t)
{
}

