/*
 * $File func_drawer.h
 * $Date: Sat Feb 05 18:15:26 2011 +0800
 *
 * func_drawer class inherited from Gtk::DrawingArea
 *
 */

#ifndef _HEADER_FUNC_DRAWER_
#define _HEADER_FUNC_DRAWER_

#include "function.h"
#include "selection_area.h"

#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <gdkmm/general.h>
#include <string>

class FuncDrawer : public SelectionArea, public Function::FillImageProgressReporter
{
	public:
		explicit FuncDrawer(const Function &func);
		virtual ~FuncDrawer();

		void save_to_bmp(const std::string &fpath);

		void zoom_out(double factor);
		// @factor > 1 for zoom out, < 1 for zoom in

		const Rectangle& get_domain();
		void set_domain(const Rectangle &domain);

		// implementation for Function::FillImageProgressReporter
		void report(double progress);
		bool test_abort();
	
	private:
		struct Render_param_t
		{
			Rectangle domain;
			int width, height;	// size of rendered image

			Render_param_t(const Rectangle &d, int w, int h);
			Render_param_t() {}
			bool operator == (const Render_param_t &n) const;
			Render_param_t& operator = (const Render_param_t &n)
			{ domain = n.domain; width = n.width; height = n.height; return *this; }
			void adjust();
			// shrink the domain if necessary so that
			// domain.width / domain.height == width / height
		};
		bool render_pixbuf(const Render_param_t &param);
		// generate a pixbuf with specified width and height,
		// using function values in @domain and update m_prev_domain
		// return true if current pixbuf is valid,
		//     or false while creating a new thread rendering the pixbuf

		void render_pixbuf_do();
		// thread for rendering the pixbuf
		// use m_cur_rparam as the parameter

		void draw_pixbuf(int x, int y, int width, int height);
		void draw_rpbar(int x, int y, int width, int height);
		// draw the rendered pixbuf or rendering progress bar
		// in the specified rectangle, or the whole DrawingArea
		// if called with (0, 0, 0, 0)


		bool on_expose_event(GdkEventExpose *event);
		void on_select(const Rectangle &area);

		// function to be graphed
		const Function &m_func;
		Rectangle m_func_domain;

		// parameter for previous successfully rendered image
		// and the image currently being rendered
		Render_param_t m_prev_rparam, m_cur_rparam;

		// pixbuf to store rendered image
		Glib::RefPtr<Gdk::Pixbuf> m_p_pixbuf;

		// rendering thread should exit if this flag is set
		bool m_render_thread_exit_flag;

		double m_render_progress;
		Glib::Dispatcher m_sig_progress, m_sig_render_done;

		Glib::Thread *m_p_render_thread; // NULL iff not rendering now
		Glib::Mutex m_mutex;
};

#endif
