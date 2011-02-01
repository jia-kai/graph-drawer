/*
 * $File func_drawer.h
 * $Date: Mon Jan 31 23:53:25 2011 +0800
 *
 * func_drawer class inherited from Gtk::DrawingArea
 *
 */

#ifndef _HEADER_FUNC_DRAWER_
#define _HEADER_FUNC_DRAWER_

#include "function.h"
#include <gtkmm/image.h>

class FuncDrawer : public Gtk::Image
{
	public:
		explicit FuncDrawer(const Function &func);
		virtual ~FuncDrawer();
		void redraw();
	
	private:
		void draw(const Rectangle &domain, int width, int height);
		// draw the graph in @domain and update m_prev_domain

		const Function &m_func;
		Rectangle m_prev_domain;
		int m_prev_width, m_prev_height;
};

#endif
