/*
 * $File func_drawer.h
 * $Date: Wed Feb 02 22:45:26 2011 +0800
 *
 * func_drawer class inherited from Gtk::DrawingArea
 *
 */

#ifndef _HEADER_FUNC_DRAWER_
#define _HEADER_FUNC_DRAWER_

#include "function.h"
#include <gtkmm/drawingarea.h>
#include <glibmm/thread.h>
#include <gdkmm/general.h>

class FuncDrawer : public Gtk::DrawingArea
{
	public:
		explicit FuncDrawer(const Function &func);
		virtual ~FuncDrawer();
	
	private:
		void render_pixbuf(const Rectangle &domain, int width, int height);
		// generate a pixbuf with specified width and height,
		// using function values in @domain and update m_prev_domain

		void render_pixbuf_do(const Rectangle &domain, int width, int height);

		bool on_expose_event(GdkEventExpose* event);


		class RenderProgressBar;


		// function to be graphed
		const Function &m_func;

		// previous function domain and image size
		Rectangle m_prev_domain;
		int m_prev_width, m_prev_height;

		// rendered pixbuf
		Glib::RefPtr<Gdk::Pixbuf> m_p_pixbuf;

		Glib::Thread *m_p_render_thread;
		Glib::Mutex m_mutex;

		// progress bar for rendering
		RenderProgressBar *m_p_rpbar; // RP ++ (ignore if puzzled)
};

#endif
