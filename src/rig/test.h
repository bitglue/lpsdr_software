#ifndef LPSDR_RIG_TEST_H
#define LPSDR_RIG_TEST_H

#include "rig.h"
#include "test_source.h"
#include <iostream>

class TestRig : public LPSDRRig {
public:
  TestRig();
  virtual ~TestRig();
  gr::basic_block_sptr source() { return m_source; };

  void set_freq(double f);
  double get_freq() { return m_freq; }
  void show_settings_window();

protected:
  double m_freq;
  test_source::sptr m_source;
  Gtk::Window *m_settings_window;
};

#endif
