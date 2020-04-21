#ifndef LPSDR_APPLICATIONCONTROLLER_H
#define LPSDR_APPLICATIONCONTROLLER_H

#include "filterwidget.h"
#include "flowgraph.h"
#include "freq_db.h"
#include "mode/mode.h"
#include "rig/rig.h"
#include "waterfall.h"
#include <glibmm/binding.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/overlay.h>
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
  Gtk::ToggleButton *run_button;
  Gtk::Scale *range_scale, *sensitivity_scale;
  Gtk::Button *rig_settings_button, *band_down_button, *band_up_button;
  Gtk::ComboBoxText *rig_selector, *mode_combo;
  Gtk::SpinButton *m_freq_spin;
  Gtk::Box *m_mode_box, *m_settings_button_box;
  Gtk::Overlay *m_overlay;

  // We don't use a Builder for our derived widgets because they have
  // properties, and properties in a builder require glibmm 2.62 or newer.
  // https://developer.gnome.org/gtkmm/stable/classGtk_1_1Builder.html
  FilterWidget m_filter_area;
  Waterfall waterfall;

  Glib::RefPtr<Gtk::Adjustment> sensitivity_adjustment, range_adjustment,
      freq_adjustment;

  // We hold references to bindings we create to keep them from going out of
  // scope, but don't really do anything with them later.
  std::vector<Glib::RefPtr<Glib::Binding>> m_bindings;

  // Bindings to the mode, which we can clear when the mode changes.
  std::vector<Glib::RefPtr<Glib::Binding>> m_mode_bindings;

protected:
  void create_bindings();
  bool on_delete_main_window();
  void on_run_button_toggled();
  void on_fft_done(float *, unsigned);
  void on_range_changed();
  void on_sensitivity_changed();
  void on_freq_changed();
  void on_rig_changed();
  void on_rig_settings_clicked();
  void on_mode_changed();
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
