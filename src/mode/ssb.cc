#include "mode/ssb.h"
#include <gnuradio/filter/firdes.h>

SSBDemod::sptr SSBDemod::make() {
  return gnuradio::get_initial_sptr(new SSBDemod());
}

SSBDemod::SSBDemod()
    : gr::hier_block2("SSBDemod",
                      gr::io_signature::make(1, 1, sizeof(gr_complex)),
                      gr::io_signature::make(1, 1, sizeof(float))) {

  // TODO: don't hardcode parameters
  auto taps = make_taps(48000, 300, 3000, 300);
  std::cout << "taps length: " << taps.size() << "\n";
  m_filter = gr::filter::fir_filter_ccc::make(1, taps);
  m_to_float = gr::blocks::complex_to_float::make();
  connect(self(), 0, m_filter, 0);
  connect(m_filter, 0, m_to_float, 0);
  connect(m_to_float, 0, self(), 0);
}

SSBDemod::~SSBDemod() {}

std::vector<gr_complex> SSBDemod::make_taps(double samp_rate, double low_cutoff,
                                            double high_cutoff,
                                            double transition) {
  return gr::filter::firdes::complex_band_pass(1, samp_rate, low_cutoff,
                                               high_cutoff, transition);
}

SSB::sptr SSB::make() { return SSB::sptr(new SSB()); }
gr::basic_block_sptr SSB::demod() { return SSBDemod::make(); }
