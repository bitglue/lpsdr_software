#ifndef LPSDR_RIG_TEST_H
#define LPSDR_RIG_TEST_H

#include "test_source.h"
#include <iostream>

class TestRig : public LPSDRRig {
public:
  TestRig() { m_source = test_source::make(); }
  gr::basic_block_sptr source() { return m_source; };

  void set_freq(double f) {
    std::cout << "test rig frequency changed to " << f << "\n";
    m_freq = f;
    m_source->set_freq(f);
  };

  double get_freq() { return m_freq; }

protected:
  double m_freq;
  test_source::sptr m_source;
};

#endif
