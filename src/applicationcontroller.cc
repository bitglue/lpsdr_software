#include "applicationcontroller.h"
#include "config.h"

#include <iostream>

ApplicationController::ApplicationController() {
  flowgraph = Flowgraph::make();

  builder = Gtk::Builder::create_from_file("src/ui.glade");
  builder->get_widget("start_button", start_button);
  builder->get_widget("stop_button", stop_button);
  builder->get_widget_derived("waterfall", waterfall);
  builder->get_widget("mainwindow", main_window);

  start_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ApplicationController::on_start_button_pressed));
  stop_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ApplicationController::on_stop_button_pressed));

  flowgraph->signal_fft_done().connect(
      sigc::mem_fun(*this, &ApplicationController::on_fft_done));
}

ApplicationController::~ApplicationController() {}

bool ApplicationController::on_delete_main_window() {
  flowgraph->stop();
  flowgraph->wait();
  return true;
}

void ApplicationController::on_start_button_pressed() {
  flowgraph->start(4096);
  std::cout << "flowgraph started\n";
}

void ApplicationController::on_stop_button_pressed() {
  flowgraph->stop();
  std::cout << "flowgraph stopped\n";
}

void ApplicationController::on_fft_done(float *magnitudes, unsigned size) {
  waterfall->add_fft(magnitudes, size);
}
