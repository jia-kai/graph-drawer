/*
 * $File: main.cpp
 * $Date: Sat Feb 05 19:59:15 2011 +0800
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
#include <glibmm/thread.h>

int main(int argc, char **argv)
{
	if (!Glib::thread_supported())
		Glib::thread_init();

	Gtk::Main kit(argc, argv);
	Gtk::Window win;
	win.set_title("Graph Drawer");
	win.set_default_size(400, 400);

	Function_mandelbrot func;
	FuncDrawer drawer(func);
	win.add(drawer);
	drawer.show();

	Gtk::Main::run(win);

	return 0;
}

/*
	this->add_events(Gdk::KEY_PRESS_MASK);
	this->set_can_focus();
	*/
