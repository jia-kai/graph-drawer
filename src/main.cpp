/*
 * $File: main.cpp
 * $Date: Sat Feb 05 12:23:18 2011 +0800
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
