#include "iq_audio_source.h"
#include <gnuradio/io_signature.h>

static const int sample_rate = 48000;

iq_audio_source::sptr iq_audio_source::make() {
  return gnuradio::get_initial_sptr(new iq_audio_source_impl());
}

iq_audio_source_impl::iq_audio_source_impl()
    : gr::hier_block2("iq_audio_source", gr::io_signature::make(0, 0, 0),
                      gr::io_signature::make(1, 1, sizeof(gr_complex))) {
  audio_source = gr::audio::source::make(sample_rate);
  delay = gr::blocks::delay::make(sizeof(float), 1);
  float_to_complex = gr::blocks::float_to_complex::make();
  message_debug = gr::blocks::message_debug::make();

  // 40m
  iqbal_fix = gr::iqbalance::fix_cc::make(-0.0530295558, -0.300588399);

  // 20m
  // iqbal_fix = gr::iqbalance::fix_cc::make(-0.111521058, -0.090425238);

  iqbal_optimize = gr::iqbalance::optimize_c::make(0);

  connect(audio_source, 0, delay, 0);
  connect(delay, 0, float_to_complex, 0);
  connect(audio_source, 1, float_to_complex, 1);

  connect(float_to_complex, 0, iqbal_fix, 0);

  connect(float_to_complex, 0, iqbal_optimize, 0);
  msg_connect(iqbal_optimize, "iqbal_corr", iqbal_fix, "iqbal_corr");
  msg_connect(iqbal_optimize, "iqbal_corr", message_debug, "print");

  connect(iqbal_fix, 0, self(), 0);
}

/*
 * Our virtual destructor.
 */
iq_audio_source_impl::~iq_audio_source_impl() {}
