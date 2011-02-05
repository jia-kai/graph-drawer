/*
 * $File: selection_area.cpp
 * $Date: Sat Feb 05 18:39:33 2011 +0800
 */

#include "selection_area.h"

#include <algorithm>
using namespace std;

SelectionArea::SelectionArea() :
	m_is_selecting(false), m_ratio(0)
{
	this->add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
}

SelectionArea::~SelectionArea()
{
}

void SelectionArea::draw_selection()
{
	Glib::RefPtr<Gdk::Window> win = this->get_window();
	Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
	gc->set_function(Gdk::INVERT);

	win->draw_rectangle(gc, false,
			(int)m_selection.x,
			(int)m_selection.y,
			(int)m_selection.width,
			(int)m_selection.height);
}

bool SelectionArea::on_button_press_event(GdkEventButton *event)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		if (event->button == 1)
		{
			// left button press
			if (m_is_selecting)
				return true;

			m_is_selecting = true;
			m_x0 = m_selection.x = event->x;
			m_y0 = m_selection.y = event->y;
			m_selection.width = m_selection.height = 0;
		} else if (event->button == 3)
		{
			// right button press, cancel selection
			if (m_is_selecting)
			{
				m_is_selecting = false;
				draw_selection();
			}
		}
	}
	return true;
}

bool SelectionArea::on_button_release_event(GdkEventButton *event)
{
	if (event->type == GDK_BUTTON_RELEASE && event->button == 1)
	{
		// left button release
		if (!m_is_selecting)
			return true;
		m_is_selecting = false;
		draw_selection();
		on_select(m_selection);
	}
	return true;
}

bool SelectionArea::on_motion_notify_event(GdkEventMotion *event)
{
	if (m_is_selecting)
	{
		draw_selection();

		double width = event->x - m_x0,
			   height = event->y - m_y0;
		if (fabs(width) < EPS || fabs(height) < EPS)
		{
			m_selection.width = m_selection.height = 0;
			return true;
		}

		if (m_ratio > EPS)
		{
			double r = fabs(width / height);
			if (fabs(r - m_ratio) > EPS)
			{
				if (r > m_ratio)
					width *= m_ratio / r;
				else
					height *= r / m_ratio;
			}
		}

		if (width > 0)
			m_selection.x = m_x0, m_selection.width = width;
		else
			m_selection.x = m_x0 + width, m_selection.width = -width;

		if (height > 0)
			m_selection.y = m_y0, m_selection.height = height;
		else
			m_selection.y = m_y0 + height, m_selection.height = -height;

		draw_selection();
	}
	return true;
}

void SelectionArea::set_ratio(Real_t ratio)
{
	m_ratio = ratio;
}

