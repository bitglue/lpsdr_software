#ifndef LPSDR_APPLICATIONCONTROLLER_H
#define LPSDR_APPLICATIONCONTROLLER_H

#include "flowgraph.h"
#include "rig/rig.h"
#include "waterfall.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/window.h>

class ApplicationController {
public:
  ApplicationController();
  virtual ~ApplicationController();

  // widgets loaded by Gtk::Builder
  Gtk::Window *main_window;
  Waterfall *waterfall;
  Gtk::ToggleButton *run_button;
  Gtk::SpinButton *range_spin, *sensitivity_spin, *delay_spin;
  Gtk::ComboBoxText *rig_selector;
  Glib::RefPtr<Gtk::Adjustment> sensitivity_adjustment, range_adjustment,
      delay_adjustment;

protected:
  std::unique_ptr<LPSDRRig> rig;
  bool on_delete_main_window();
  void on_run_button_toggled();
  void on_fft_done(float *, unsigned);
  void on_range_changed();
  void on_delay_changed();
  void on_sensitivity_changed();
  void on_freq_changed();
  void on_rig_changed();

  Flowgraph::sptr flowgraph;
};

#endif
