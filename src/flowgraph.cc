#include "flowgraph.h"
#include "config.h"

Flowgraph::Flowgraph(Waterfall &waterfall) : gr::top_block(PACKAGE_NAME) {
  audio_source = gr::audio::source::make(48000);
  float_to_complex = gr::blocks::float_to_complex::make();
  waterfall_sink = dispatcher_sink::make(waterfall);

  connect(audio_source, 0, float_to_complex, 0);
  connect(audio_source, 1, float_to_complex, 1);

  connect(float_to_complex, 0, waterfall_sink, 0);
};

Flowgraph::~Flowgraph(){};
