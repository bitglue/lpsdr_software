#include <gtkmm/builder.h>

#include "test.h"

TestRig::TestRig() {
  m_source = test_source::make();

  auto builder = Gtk::Builder::create_from_file("src/test_rig.glade");
  builder->get_widget("settings_window", m_settings_window);
}

TestRig::~TestRig() { delete (m_settings_window); }

void TestRig::set_freq(double f) {
  std::cout << "test rig frequency changed to " << f << "\n";
  m_freq = f;
  m_source->set_freq(f);
}

void TestRig::show_settings_window() { m_settings_window->show(); }
