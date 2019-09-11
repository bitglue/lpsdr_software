#ifndef LPSDR_RIG_TEST_H
#define LPSDR_RIG_TEST_H

#include "test_source.h"
#include <iostream>

class TestRig : public LPSDRRig {
public:
  gr::basic_block_sptr source() { return test_source::make(); };
  void set_freq(double f){ std::cout << "test rig frequency changed to " << f << "\n"; };
};

#endif
