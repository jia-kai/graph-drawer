/*
 * $File func_drawer.h
 * $Date: Tue Feb 01 16:41:08 2011 +0800
 *
 * func_drawer class inherited from Gtk::DrawingArea
 *
 */

#ifndef _HEADER_FUNC_DRAWER_
#define _HEADER_FUNC_DRAWER_

#include "function.h"
#include <gtkmm/drawingarea.h>

class FuncDrawer : public Gtk::DrawingArea
{
	public:
		explicit FuncDrawer(const Function &func);
		virtual ~FuncDrawer();
	
	private:
		void gen_pixbuf(const Rectangle &domain, int width, int height);
		// generate a pixbuf with specified width and height,
		// using function values in @domain and update m_prev_domain

		bool on_expose_event(GdkEventExpose* event);

		const Function &m_func;
		Rectangle m_prev_domain;
		int m_prev_width, m_prev_height;
		Glib::RefPtr<Gdk::Pixbuf> m_p_pixbuf;
};

#endif
