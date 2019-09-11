#include "applicationcontroller.h"
#include "config.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "com.bitglue.lpsdr");
  auto app_controller = ApplicationController();

  return app->run(*app_controller.main_window);
}
