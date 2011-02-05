/*
 * $File: selection_area.cpp
 * $Date: Sat Feb 05 18:16:38 2011 +0800
 */

#include "selection_area.h"

SelectionArea::SelectionArea()
{
	this->add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
}

SelectionArea::~SelectionArea()
{
}

bool SelectionArea::on_button_press_event(GdkEventButton *event)
{
	printf("press\n");
	return true;
	/*
	if (event->type == Gdk::BUTTON_PRESS && event->button == 1)
	{
		// left button press
		if (m_is_selecting)
			return true;

		{
			LOCK;
			if (m_p_render_thread)
				return true;
		}

		m_is_selecting = true;
		m_selection.x = event->x;
		m_selection.y = event->y;
		m_selection.width = m_selection.height = 0;
	}
	return true;
	*/
}

bool SelectionArea::on_button_release_event(GdkEventButton *event)
{
	printf("release\n");
	/*
	if (event->type == Gdk::BUTTON_RELEASE && event->button == 1)
	{
		// left button release
		if (!m_is_selecting)
			return true;
		m_is_selecting = false;

		{
			LOCK;
			if (m_p_render_thread)
				return true;
		}

		if (m_is_selecting)
		{
			m_is_selecting = true;
			m_selection.x = event->x;
			m_selection.y = event->y;
			m_selection.width = m_selection.height = 0;
		}
	}
	return true;
	*/
}

bool SelectionArea::on_motion_notify_event(GdkEventMotion *event)
{
	return true;
	/*
	if (m_is_selecting)
	{
		Glib::RefPtr<Gdk::Window> win = this->get_window();
		Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
		gc->set_function(Gdk::GC::INVERT);
	}
	return true;
	*/
}


