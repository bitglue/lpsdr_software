#ifndef LPSDR_RIG_IQ_AUDIO_SOURCE_H
#define LPSDR_RIG_IQ_AUDIO_SOURCE_H

#include <gnuradio/audio/source.h>
#include <gnuradio/blocks/delay.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/message_debug.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/iqbalance/fix_cc.h>
#include <gnuradio/iqbalance/optimize_c.h>

class iq_audio_source : virtual public gr::hier_block2 {
public:
  typedef boost::shared_ptr<iq_audio_source> sptr;
  static sptr make();
  virtual void set_dly(int d) = 0;
};

class iq_audio_source_impl : public iq_audio_source {
public:
  iq_audio_source_impl();
  ~iq_audio_source_impl();
  void set_dly(int d);

protected:
  gr::audio::source::sptr audio_source;
  gr::blocks::delay::sptr delay;
  gr::blocks::float_to_complex::sptr float_to_complex;
  gr::blocks::message_debug::sptr message_debug;
  gr::iqbalance::fix_cc::sptr iqbal_fix;
  gr::iqbalance::optimize_c::sptr iqbal_optimize;
};

#endif
