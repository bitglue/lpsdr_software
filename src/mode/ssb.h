#ifndef LPSDR_MODE_SSB_H
#define LPSDR_MODE_SSB_H

#include "mode/mode.h"
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/filter/fir_filter_ccc.h>
#include <gnuradio/hier_block2.h>

class SSB : virtual public Mode {
public:
  typedef std::shared_ptr<SSB> sptr;
  static sptr make();
  ~SSB(){};
  gr::basic_block_sptr demod();
};

class SSBDemod : virtual public gr::hier_block2 {
public:
  typedef boost::shared_ptr<SSBDemod> sptr;
  static sptr make();

private:
  SSBDemod();
  virtual ~SSBDemod();
  static std::vector<gr_complex> make_taps(double samp_rate, double low_cutoff,
                                           double high_cutoff,
                                           double transition);
  gr::filter::fir_filter_ccc::sptr m_filter;
  gr::blocks::complex_to_float::sptr m_to_float;
};

#endif
