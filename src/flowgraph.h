#ifndef LIBSDR_FLOWGRAPH_H
#define LIBSDR_FLOWGRAPH_H

#include "dispatcher_sink.h"
#include "waterfall.h"
#include <gnuradio/audio/source.h>
#include <gnuradio/blocks/delay.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/message_debug.h>
#include <gnuradio/iqbalance/fix_cc.h>
#include <gnuradio/iqbalance/optimize_c.h>
#include <gnuradio/top_block.h>

class Flowgraph : public gr::top_block {
public:
  Flowgraph(Waterfall &waterfall);
  virtual ~Flowgraph();

private:
  gr::audio::source::sptr audio_source;
  gr::blocks::delay::sptr delay;
  gr::blocks::float_to_complex::sptr float_to_complex;
  gr::blocks::message_debug::sptr message_debug;
  gr::iqbalance::fix_cc::sptr iqbal_fix;
  gr::iqbalance::optimize_c::sptr iqbal_optimize;
  dispatcher_sink::sptr waterfall_sink;
};

#endif
