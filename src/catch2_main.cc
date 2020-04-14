#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "gresources.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
  gresources_register_resource();
  auto app = Gtk::Application::create();

  int result = Catch::Session().run(argc, argv);

  return result;
}
