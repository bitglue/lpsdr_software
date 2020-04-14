#include "applicationcontroller.h"
#include "config.h"
#include "mode/ssb.h"
#include "rig/iqonly.h"
#include "rig/softrock.h"
#include "rig/test.h"
#include <gtkmm/builder.h>

#ifdef HAVE_BREADBOARD
#include "rig/breadboard.h"
#endif

ApplicationController::ApplicationController()
    : sensitivity_adjustment(Gtk::Adjustment::create(-110, -120, 0)),
      range_adjustment(Gtk::Adjustment::create(40, 1, 100)),
      freq_adjustment(Gtk::Adjustment::create(14e6, 0, 30e6)),
      rig(new TestRig()) {
  flowgraph = Flowgraph::make(rig->source());

  auto builder =
      Gtk::Builder::create_from_resource("/com/bitglue/LPSDR/ui.glade");
  builder->get_widget("range_scale", range_scale);
  builder->get_widget("sensitivity_scale", sensitivity_scale);
  builder->get_widget("run_button", run_button);
  builder->get_widget("band_down_button", band_down_button);
  builder->get_widget("band_up_button", band_up_button);
  builder->get_widget("rig_settings_button", rig_settings_button);
  builder->get_widget_derived("waterfall", waterfall);
  builder->get_widget("mainwindow", main_window);
  builder->get_widget("rig_selector", rig_selector);
  builder->get_widget("mode_combo", mode_combo);
  builder->get_widget("freq_spin", m_freq_spin);
  builder->get_widget("mode_box", m_mode_box);

  range_scale->set_adjustment(range_adjustment);
  sensitivity_scale->set_adjustment(sensitivity_adjustment);
  m_freq_spin->set_adjustment(freq_adjustment);
  waterfall->set_adjustment(freq_adjustment);

  mode_combo->append("LSB");
  mode_combo->append("USB");

  run_button->signal_toggled().connect(
      sigc::mem_fun(*this, &ApplicationController::on_run_button_toggled));
  band_down_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ApplicationController::band_down));
  band_up_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ApplicationController::band_up));
  rig_settings_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ApplicationController::on_rig_settings_clicked));
  range_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_range_changed));
  sensitivity_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_sensitivity_changed));
  flowgraph->signal_fft_done().connect(
      sigc::mem_fun(*this, &ApplicationController::on_fft_done));
  freq_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_freq_changed));
  rig_selector->signal_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_rig_changed));
  mode_combo->signal_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_mode_changed));

  flowgraph->enable_udp_debug("192.168.1.113", 1234, 1472, false);
  mode_combo->set_active_text("USB");
  on_sensitivity_changed();
  on_range_changed();
  on_run_button_toggled();
}

ApplicationController::~ApplicationController() {
  // only top-level objects from the builder must be freed
  delete main_window;
}

bool ApplicationController::on_delete_main_window() {
  flowgraph->stop();
  flowgraph->wait();
  return true;
}

void ApplicationController::on_run_button_toggled() {
  if (run_button->get_active()) {
    flowgraph->start(4096);
  } else {
    flowgraph->stop();
    flowgraph->wait();
  }
}

void ApplicationController::on_fft_done(float *magnitudes, unsigned size) {
  waterfall->add_fft(magnitudes, size);
}

void ApplicationController::on_range_changed() {
  waterfall->set_range(range_adjustment->get_value());
}

void ApplicationController::on_sensitivity_changed() {
  waterfall->set_sensitivity(sensitivity_adjustment->get_value());
}

void ApplicationController::on_freq_changed() {
  rig->set_freq(freq_adjustment->get_value());
}

void ApplicationController::on_rig_changed() {
  auto selected_rig = rig_selector->get_active_text();
  if (selected_rig == "Test") {
    rig.reset(new TestRig());
  } else if (selected_rig == "Softrock") {
    rig.reset(new Softrock());
#ifdef HAVE_BREADBOARD
  } else if (selected_rig == "Breadboard") {
    rig.reset(new Breadboard());
#endif
  } else if (selected_rig == "IQ Only") {
    rig.reset(new IQOnly());
  }
  on_freq_changed();
  flowgraph->set_source(rig->source());
}

void ApplicationController::on_rig_settings_clicked() {
  rig->show_settings_window();
}

void ApplicationController::on_mode_changed() {
  auto selected_mode = mode_combo->get_active_text();
  if (selected_mode == "USB") {
    set_mode(SSB::make(false));
  } else if (selected_mode == "LSB") {
    set_mode(SSB::make(true));
  }
}

void ApplicationController::set_mode(std::shared_ptr<Mode> mode) {
  auto children = m_mode_box->get_children();
  for (unsigned i = 2; i < children.size(); i++) {
    auto child = children.at(i);
    m_mode_box->remove(*child);
  }
  m_mode = mode;
  m_mode_box->add(mode->settings_widget());
  m_mode_box->show_all();
  flowgraph->set_demod(mode->demod());
}

void ApplicationController::band_down() {
  tune_to_band(m_freq_db.band_below(freq_adjustment->get_value()));
}

void ApplicationController::band_up() {
  tune_to_band(m_freq_db.band_above(freq_adjustment->get_value()));
}

void ApplicationController::tune_to_band(const Band &b) {
  auto f = b.center();
  freq_adjustment->set_value(f);
}
