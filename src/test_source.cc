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
      gr::analog::noise_type_t::GR_UNIFORM, 0.001);
  throttle = gr::blocks::throttle::make(sizeof(gr_complex), sample_rate);

  connect(noise_source, 0, throttle, 0);
  connect(throttle, 0, self(), 0);
}

/*
 * Our virtual destructor.
 */
test_source_impl::~test_source_impl() {}
