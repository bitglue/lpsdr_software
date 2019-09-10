#ifndef LPSDR_APPLICATIONCONTROLLER_H
#define LPSDR_APPLICATIONCONTROLLER_H

#include "flowgraph.h"
#include "waterfall.h"
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

class ApplicationController {
public:
  ApplicationController();
  virtual ~ApplicationController();

  // widgets loaded by Gtk::Builder
  Gtk::Window *main_window;
  Waterfall *waterfall;
  Gtk::Button *start_button;
  Gtk::Button *stop_button;

protected:
  Glib::RefPtr<Gtk::Builder> builder;
  bool on_delete_main_window();
  void on_start_button_pressed();
  void on_stop_button_pressed();
  void on_fft_done(float *, unsigned);

  Flowgraph::sptr flowgraph;
};

#endif
