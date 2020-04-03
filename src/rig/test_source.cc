#include "test_source.h"
#include <gnuradio/io_signature.h>

static const int sample_rate = 48000;

test_source::sptr test_source::make() {
  return gnuradio::get_initial_sptr(new test_source_impl());
}

test_source_impl::test_source_impl()
    : gr::hier_block2("test_source", gr::io_signature::make(0, 0, 0),
                      gr::io_signature::make(1, 1, sizeof(gr_complex))) {
  noise_source = gr::analog::noise_source_c::make(
      gr::analog::noise_type_t::GR_UNIFORM, 0.0004);
  sig_source = gr::analog::sig_source_c::make(48000.0, gr::analog::GR_COS_WAVE,
                                              -10000.0, 0.002);
  throttle = gr::blocks::throttle::make(sizeof(gr_complex), sample_rate);
  add = gr::blocks::add_cc::make();

  connect(noise_source, 0, add, 0);
  connect(sig_source, 0, add, 1);
  connect(add, 0, throttle, 0);
  connect(throttle, 0, self(), 0);
}

void test_source_impl::set_freq(double f) {
  sig_source->set_frequency(7000.0 - f);
}

/*
 * Our virtual destructor.
 */
test_source_impl::~test_source_impl() {}
