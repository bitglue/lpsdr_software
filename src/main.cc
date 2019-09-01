#include "config.h"
#include "mainwindow.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
  int status;
  auto app = Gtk::Application::create(argc, argv, "com.bitglue.lpsdr");

  MainWindow mainwindow;
  return app->run(mainwindow);
}
