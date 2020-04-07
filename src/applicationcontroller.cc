#include <gtkmm/builder.h>

#include "applicationcontroller.h"
#include "config.h"
#include "rig/iqonly.h"
#include "rig/softrock.h"
#include "rig/test.h"

#ifdef HAVE_BREADBOARD
#include "rig/breadboard.h"
#endif

ApplicationController::ApplicationController()
    : sensitivity_adjustment(Gtk::Adjustment::create(-110, -120, 0)),
      range_adjustment(Gtk::Adjustment::create(40, 1, 100)),
      rig(new TestRig()) {
  flowgraph = Flowgraph::make(rig->source());

  auto builder = Gtk::Builder::create_from_file("src/ui.glade");
  builder->get_widget("range", range_spin);
  builder->get_widget("sensitivity", sensitivity_spin);
  builder->get_widget("run_button", run_button);
  builder->get_widget("rig_settings_button", rig_settings_button);
  builder->get_widget_derived("waterfall", waterfall);
  builder->get_widget("mainwindow", main_window);
  builder->get_widget("rig_selector", rig_selector);

  range_spin->set_adjustment(range_adjustment);
  sensitivity_spin->set_adjustment(sensitivity_adjustment);

  run_button->signal_toggled().connect(
      sigc::mem_fun(*this, &ApplicationController::on_run_button_toggled));
  rig_settings_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ApplicationController::on_rig_settings_clicked));
  range_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_range_changed));
  sensitivity_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_sensitivity_changed));
  flowgraph->signal_fft_done().connect(
      sigc::mem_fun(*this, &ApplicationController::on_fft_done));
  waterfall->signal_freq_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_freq_changed));
  rig_selector->signal_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_rig_changed));

  flowgraph->enable_udp_debug("192.168.1.113", 1234, 1472, false);
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
  rig->set_freq(waterfall->get_center_freq());
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
