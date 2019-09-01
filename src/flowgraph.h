#ifndef LIBSDR_FLOWGRAPH_H
#define LIBSDR_FLOWGRAPH_H

#include "dispatcher_sink.h"
#include "waterfall.h"
#include <gnuradio/audio/source.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/top_block.h>

class Flowgraph : public gr::top_block {
public:
  Flowgraph(Waterfall &waterfall);
  virtual ~Flowgraph();

private:
  gr::audio::source::sptr audio_source;
  gr::blocks::float_to_complex::sptr float_to_complex;
  dispatcher_sink::sptr waterfall_sink;
};

#endif
