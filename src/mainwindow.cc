#include "mainwindow.h"

MainWindow::MainWindow() : waterfall(), flowgraph(waterfall) {
  set_default_size(700, 300);
  set_border_width(0);
  fullscreen();
  add(waterfall);
  flowgraph.start(4096);
  show_all();
}

bool MainWindow::on_delete_event(GdkEventAny *any_event) {
  flowgraph.stop();
  return Gtk::Window::on_delete_event(any_event);
}

MainWindow::~MainWindow() {}
