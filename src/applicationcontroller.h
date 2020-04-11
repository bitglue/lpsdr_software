#ifndef LPSDR_APPLICATIONCONTROLLER_H
#define LPSDR_APPLICATIONCONTROLLER_H

#include "flowgraph.h"
#include "freq_db.h"
#include "mode/mode.h"
#include "rig/rig.h"
#include "waterfall.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/scale.h>
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
  Gtk::Scale *range_scale, *sensitivity_scale;
  Gtk::Button *rig_settings_button, *band_down_button, *band_up_button;
  Gtk::ComboBoxText *rig_selector;
  Gtk::SpinButton *m_freq_spin;
  Glib::RefPtr<Gtk::Adjustment> sensitivity_adjustment, range_adjustment,
      freq_adjustment;

protected:
  bool on_delete_main_window();
  void on_run_button_toggled();
  void on_fft_done(float *, unsigned);
  void on_range_changed();
  void on_sensitivity_changed();
  void on_freq_changed();
  void on_rig_changed();
  void on_rig_settings_clicked();
  void set_mode(std::shared_ptr<Mode>);
  void band_down();
  void band_up();
  void tune_to_band(const Band &);

  FrequencyDB m_freq_db;
  Flowgraph::sptr flowgraph;
  std::unique_ptr<LPSDRRig> rig;
  std::shared_ptr<Mode> m_mode;
};

#endif
