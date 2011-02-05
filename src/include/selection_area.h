/*
 * $File: selection_area.h
 * $Date: Sat Feb 05 18:36:29 2011 +0800
 *
 * a DrawingArea that enables the user to select some rectangular area
 *
 */

#include "common.h"

#include <gtkmm/drawingarea.h>

class SelectionArea : public Gtk::DrawingArea
{
	protected:
		SelectionArea();
		virtual ~SelectionArea();

		virtual void on_select(const Rectangle &area) = 0;
		// callback function when a user selects some area

		void set_ratio(Real_t ratio);
		// set the width / height ratio of selection area,
		// or 0 if not restricted

	private:
		bool on_button_press_event(GdkEventButton *event);
		bool on_button_release_event(GdkEventButton *event);
		bool on_motion_notify_event(GdkEventMotion *event);

		void draw_selection();

		bool m_is_selecting;
		double m_x0, m_y0;
		Rectangle m_selection;

		Real_t m_ratio;
};

