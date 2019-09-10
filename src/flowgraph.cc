#include "flowgraph.h"
#include "config.h"
#include "test_source.h"

Flowgraph::sptr Flowgraph::make() {
  return gnuradio::get_initial_sptr(new Flowgraph());
}

Flowgraph::Flowgraph() : gr::top_block(PACKAGE_NAME) {
  // source = iq_audio_source::make();
  source = test_source::make();
  waterfall_sink = dispatcher_sink::make();

  connect(source, 0, waterfall_sink, 0);
}

Flowgraph::~Flowgraph() {}

dispatcher_sink::signal_fft_done_t Flowgraph::signal_fft_done() {
  return waterfall_sink->fft_done;
}
