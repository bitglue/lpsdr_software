#include "flowgraph.h"
#include "config.h"
#include "gr_lock.h"

Flowgraph::sptr Flowgraph::make(gr::basic_block_sptr source,
                                unsigned sample_rate) {
  return gnuradio::get_initial_sptr(new Flowgraph(source, sample_rate));
}

Flowgraph::Flowgraph(gr::basic_block_sptr source, unsigned sample_rate)
    : gr::top_block(PACKAGE_NAME), m_source(source),
      m_sample_rate(sample_rate) {
  waterfall_sink = dispatcher_sink::make();
  make_connections();
}

Flowgraph::~Flowgraph() {}

dispatcher_sink::signal_fft_done_t Flowgraph::signal_fft_done() {
  return waterfall_sink->fft_done;
}

void Flowgraph::set_source(gr::basic_block_sptr source) {
  if (m_source) {
    disconnect(m_source);
  }

  m_source = source;
  make_connections();
}

void Flowgraph::enable_udp_debug(const std::string &host, int port,
                                 int payload_size, bool eof) {
  // disable existing UDP sink, if any, which might have a different
  // configuration.
  disable_udp_debug();

  m_udp_sink = gr::blocks::udp_sink::make(sizeof(gr_complex), host, port,
                                          payload_size, eof);
  make_connections();
}

void Flowgraph::disable_udp_debug() {
  if (m_udp_sink) {
    auto flowgraph_lock = gr_lock(to_top_block());
    disconnect(m_udp_sink);
    m_udp_sink = nullptr;
  }
}

void Flowgraph::set_demod(gr::basic_block_sptr demod) {
  auto flowgraph_lock = gr_lock(to_top_block());
  auto old_demod = m_demod;

  if (m_audio_sink) {
    disconnect(m_audio_sink);
    m_audio_sink = nullptr;
  }

  if (old_demod) {
    disconnect(old_demod);
  }

  m_demod = demod;
  make_connections();
}

void Flowgraph::make_connections() {
  auto flowgraph_lock = gr_lock(to_top_block());
  disconnect_all();

  connect(m_source, 0, waterfall_sink, 0);

  if (m_udp_sink) {
    connect(m_source, 0, m_udp_sink, 0);
  }

  if (m_demod) {
    connect(m_source, 0, m_demod, 0);

    // TODO: implement some interface for demodulators to specify the kind of
    // sink they need
    if (m_demod->output_signature()->min_streams()) {
      m_audio_sink = gr::audio::sink::make(m_sample_rate);
      connect(m_demod, 0, m_audio_sink, 0);
    }
  }
}
