/*
 * $File: main.cpp
 * $Date: Tue Feb 01 16:36:33 2011 +0800
 */

#include "func_drawer.h"
#include "func/mandelbrot.h"
#include <gtkmm/main.h>
#include <gtkmm/window.h>

int main(int argc, char **argv)
{
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

