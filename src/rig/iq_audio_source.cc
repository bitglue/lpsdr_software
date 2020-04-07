#include "iq_audio_source.h"
#include "gr_lock.h"
#include <gnuradio/io_signature.h>

static const int sample_rate = 48000;

iq_audio_source::sptr iq_audio_source::make(const std::string device_name) {
  return gnuradio::get_initial_sptr(new iq_audio_source_impl(device_name));
}

iq_audio_source_impl::iq_audio_source_impl(const std::string device_name)
    : gr::hier_block2("iq_audio_source", gr::io_signature::make(0, 0, 0),
                      gr::io_signature::make(1, 1, sizeof(gr_complex))) {
  audio_source = gr::audio::source::make(sample_rate, device_name, true);
  m_device_name = device_name;
  delay = gr::blocks::delay::make(sizeof(float), 0);
  float_to_complex = gr::blocks::float_to_complex::make();

  iqbal_fix = gr::iqbalance::fix_cc::make(0, 0);
  iqbal_optimize = gr::iqbalance::optimize_c::make(0);

  connect_audio_source();
  connect(delay, 0, float_to_complex, 0);

  connect(float_to_complex, 0, iqbal_fix, 0);

  connect(float_to_complex, 0, iqbal_optimize, 0);
  msg_connect(iqbal_optimize, "iqbal_corr", iqbal_fix, "iqbal_corr");

  connect(iqbal_fix, 0, self(), 0);
}

void iq_audio_source_impl::connect_audio_source() {
  connect(audio_source, 0, delay, 0);
  connect(audio_source, 1, float_to_complex, 1);
}

void iq_audio_source_impl::set_audio_device(const std::string device_name) {
  auto lock = gr_lock(this->to_hier_block2());
  auto new_audio_source =
      gr::audio::source::make(sample_rate, device_name, true);

  disconnect(audio_source);
  audio_source = new_audio_source;
  connect_audio_source();
  m_device_name = device_name;
}

iq_audio_source_impl::~iq_audio_source_impl() {}

void iq_audio_source_impl::set_dly(int d) { delay->set_dly(d); }

void iq_audio_source_impl::set_iq_correction(float mag, float phase) {
  iqbal_fix->set_mag(mag);
  iqbal_fix->set_phase(phase);
}

void iq_audio_source_impl::set_auto_iq_correction(bool enabled) {
  iqbal_optimize->reset();
  if (enabled) {
    iqbal_optimize->set_period(3000);
  } else {
    iqbal_optimize->set_period(0);
  }
}
