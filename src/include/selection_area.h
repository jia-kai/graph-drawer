/*
 * $File: selection_area.h
 * $Date: Sat Feb 05 20:00:04 2011 +0800
 *
 * a DrawingArea that enables the user to select some rectangular area
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

#ifndef _HEADER_SELECTION_AREA_
#define _HEADER_SELECTION_AREA_

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


#endif
