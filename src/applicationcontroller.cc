#include "applicationcontroller.h"
#include "config.h"

ApplicationController::ApplicationController()
    : sensitivity_adjustment(Gtk::Adjustment::create(-110, -120, 0)),
      range_adjustment(Gtk::Adjustment::create(40, 1, 100)) {
  flowgraph = Flowgraph::make();

  builder = Gtk::Builder::create_from_file("src/ui.glade");
  builder->get_widget("range", range_spin);
  builder->get_widget("sensitivity", sensitivity_spin);
  builder->get_widget("run_button", run_button);
  builder->get_widget_derived("waterfall", waterfall);
  builder->get_widget("mainwindow", main_window);

  range_spin->set_adjustment(range_adjustment);
  sensitivity_spin->set_adjustment(sensitivity_adjustment);

  run_button->signal_toggled().connect(
      sigc::mem_fun(*this, &ApplicationController::on_run_button_toggled));
  range_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_range_changed));
  sensitivity_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &ApplicationController::on_sensitivity_changed));

  flowgraph->signal_fft_done().connect(
      sigc::mem_fun(*this, &ApplicationController::on_fft_done));

  on_sensitivity_changed();
  on_range_changed();
  on_run_button_toggled();
}

ApplicationController::~ApplicationController() {}

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
