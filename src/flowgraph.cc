#include "flowgraph.h"
#include "config.h"

Flowgraph::Flowgraph(Waterfall &waterfall) : gr::top_block(PACKAGE_NAME) {
  audio_source = gr::audio::source::make(48000);
  delay = gr::blocks::delay::make(sizeof(float), 1);
  float_to_complex = gr::blocks::float_to_complex::make();
  message_debug = gr::blocks::message_debug::make();

  // 40m
  iqbal_fix = gr::iqbalance::fix_cc::make(-0.0530295558, -0.300588399);

  // 20m
  // iqbal_fix = gr::iqbalance::fix_cc::make(-0.111521058, -0.090425238);

  // iqbal_optimize = gr::iqbalance::optimize_c::make(8192*2*2);
  waterfall_sink = dispatcher_sink::make(waterfall);

  connect(audio_source, 0, delay, 0);
  connect(delay, 0, float_to_complex, 0);
  connect(audio_source, 1, float_to_complex, 1);

  connect(float_to_complex, 0, iqbal_fix, 0);

  // connect(float_to_complex, 0, iqbal_optimize, 0);
  // msg_connect(iqbal_optimize, "iqbal_corr", iqbal_fix, "iqbal_corr");
  // msg_connect(iqbal_optimize, "iqbal_corr", message_debug, "print");

  connect(iqbal_fix, 0, waterfall_sink, 0);
};

Flowgraph::~Flowgraph(){};
