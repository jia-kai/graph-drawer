/*
 * $File: main.cpp
 * $Date: Sun Feb 06 12:21:06 2011 +0800
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
#include "func/mandelbrot.h"

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>
#include <glibmm/thread.h>
#include <cstdio>

static const double ZOOM_OUT_FACTOR = 1.5;

class FuncDrawerUI : public FuncDrawer
{
	public:
		FuncDrawerUI(Function &func, Gtk::Label *label, Gtk::Window *par_win) :
			FuncDrawer(func), m_p_label(label), m_p_par_win(par_win), m_func(func)
		{
			this->add_events(Gdk::KEY_PRESS_MASK);
			this->set_can_focus();
		}

		~FuncDrawerUI()
		{
		}
	
	private:
		Gtk::Label *m_p_label;
		Gtk::Window *m_p_par_win;
		Function &m_func;

		void on_size_request(Gtk::Requisition *requisition)
		{
			requisition->width = 400;
			requisition->height = 400;
		}

		void on_cursor_motion(Real_t x, Real_t y)
		{
			char str[256];
			sprintf(str, "X: %.5le  Y: %.5le", x, y);
			m_p_label->set_text(str);
		}

		bool on_key_press_event(GdkEventKey *event)
		{
			switch (event->keyval)
			{
				case 'a':
					this->stop_render();
					break;
				case 's':
					save();
					break;
				case 'z':
					this->zoom(ZOOM_OUT_FACTOR);
					break;
				case 'Z':
					read_factor_zoom();
					break;
				default:
					m_func.on_key_press(event->keyval);
			}
			return true;
		}

		void read_factor_zoom()
		{
			double f;
			printf("Please enter a zoom factor (> 1 for zoom out): ");
			if (scanf("%lf", &f) == 1)
				this->zoom(f);
		}

		void save()
		{
			Gtk::FileChooserDialog dialog(*m_p_par_win, "Save Image", Gtk::FILE_CHOOSER_ACTION_SAVE);
			dialog.set_transient_for(*m_p_par_win);
			dialog.set_do_overwrite_confirmation();
			dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

			Gtk::FileFilter filter;
			filter.set_name("Bitmap Image File");
			filter.add_mime_type("image/bmp");
			filter.add_pattern("*.bmp");
			dialog.add_filter(filter);

			if (dialog.run() == Gtk::RESPONSE_OK)
				this->save_to_bmp(dialog.get_filename());
		}
};

int main(int argc, char **argv)
{
	if (!Glib::thread_supported())
		Glib::thread_init();
	printf(
		"key bindings:\n"
		"    s -- save image to file\n"
		"    a -- abort rendering process\n"
		"    z -- zoom out by factor %.3lf\n"
		"    Z -- read a zoom factor from stdin and zoom with it\n"
		"mouse operation:\n"
		"    1. hold the left button to draw a box to zoom in, and\n"
		"       press right button while drawing to cancel.\n"
		"    2. press right button to make the point under cursor the center.\n",
		ZOOM_OUT_FACTOR);

	Gtk::Main kit(argc, argv);
	Gtk::Window win;
	win.set_title("Graph Drawer");

	Function_mandelbrot func;

	Gtk::VBox box;
	Gtk::Label label("X: Y:");
	FuncDrawerUI drawer(func, &label, &win);
	box.pack_start(drawer);
	box.pack_end(label, false, false);


	win.add(box);
	win.show_all();

	Gtk::Main::run(win);

	return 0;
}

