/*
 * $File func_drawer.h
 * $Date: Thu Feb 03 23:09:20 2011 +0800
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
		bool render_pixbuf(const Rectangle &domain, int width, int height);
		// generate a pixbuf with specified width and height,
		// using function values in @domain and update m_prev_domain
		// return true if current pixbuf is valid,
		//     or false while creating a new thread rendering the pixbuf

		void render_pixbuf_do(const Rectangle &domain, int width, int height);

		bool on_expose_event(GdkEventExpose* event);
		void draw_pixbuf(int x, int y, int width, int height);


		class RenderProgressBar;


		// function to be graphed
		const Function &m_func;

		// previous successfully rendered function domain and image size
		Rectangle m_prev_domain;
		int m_prev_width, m_prev_height;

		// rendered pixbuf
		Glib::RefPtr<Gdk::Pixbuf> m_p_pixbuf;

		// currently rendered function image size
		int m_cur_render_width, m_cur_render_height;

		// rendering thread should exit if this flag is set
		bool m_render_thread_exit_flag;

		Glib::Thread *m_p_render_thread; // NULL iff not rendering now
		Glib::Mutex m_mutex;

		// progress bar for rendering
		RenderProgressBar *m_p_rpbar; // RP ++ (ignore if puzzled)
};

#endif
