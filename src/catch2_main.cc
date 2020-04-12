#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create();

  int result = Catch::Session().run(argc, argv);

  return result;
}
