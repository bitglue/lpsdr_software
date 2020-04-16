#ifndef LPSDR_RIG_IQ_AUDIO_SOURCE_H
#define LPSDR_RIG_IQ_AUDIO_SOURCE_H

#include <gnuradio/audio/source.h>
#include <gnuradio/blocks/delay.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/iqbalance/fix_cc.h>
#include <gnuradio/iqbalance/optimize_c.h>

class iq_audio_source : virtual public gr::hier_block2 {
public:
  typedef boost::shared_ptr<iq_audio_source> sptr;
  static sptr make(unsigned sample_rate, const std::string device_name = "");

  virtual void set_dly(int d) = 0;
  virtual void set_iq_correction(float mag, float phase) = 0;
  virtual void set_auto_iq_correction(bool enabled) = 0;
  virtual void set_audio_device(const std::string device_name) = 0;

  virtual float iq_fix_mag() = 0;
  virtual float iq_fix_phase() = 0;
  virtual const std::string device_name() const = 0;
};

class iq_audio_source_impl : public iq_audio_source {
public:
  iq_audio_source_impl(unsigned sample_rate,
                       const std::string device_name = "");
  ~iq_audio_source_impl();

  void set_dly(int d);
  void set_iq_correction(float mag, float phase);
  void set_auto_iq_correction(bool enabled);
  void set_audio_device(const std::string device_name);

  float iq_fix_mag() { return iqbal_fix->mag(); };
  float iq_fix_phase() { return iqbal_fix->phase(); };
  const std::string device_name() const { return m_device_name; };

protected:
  const unsigned m_sample_rate;
  gr::audio::source::sptr audio_source;
  gr::blocks::float_to_complex::sptr float_to_complex;

  // Some audio interfaces delay one channel which makes IQ balance
  // optimization fail to converge over the entire audio bandwidth. This delay
  // can be set to compensate.
  gr::blocks::delay::sptr delay;

  // Low-frequency noise (a lot of it mains hum) tends to be imposed on both I
  // and Q signals at low frequencies. If we don't filter it out, the IQ
  // optimization algorithm can converge on that noise, leading to a wacky
  // result.
  gr::filter::fir_filter_ccf::sptr mains_hum_filter;

  gr::iqbalance::fix_cc::sptr iqbal_fix;
  gr::iqbalance::optimize_c::sptr iqbal_optimize;

  std::string m_device_name;

  void connect_audio_source();
};

#endif
