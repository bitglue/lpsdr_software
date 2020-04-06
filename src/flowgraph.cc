#include "flowgraph.h"
#include "config.h"
#include "gr_lock.h"

Flowgraph::sptr Flowgraph::make(gr::basic_block_sptr source) {
  return gnuradio::get_initial_sptr(new Flowgraph(source));
}

Flowgraph::Flowgraph(gr::basic_block_sptr source)
    : gr::top_block(PACKAGE_NAME), m_source(source) {
  waterfall_sink = dispatcher_sink::make();

  connect(m_source, 0, waterfall_sink, 0);
}

Flowgraph::~Flowgraph() {}

dispatcher_sink::signal_fft_done_t Flowgraph::signal_fft_done() {
  return waterfall_sink->fft_done;
}

void Flowgraph::set_source(gr::basic_block_sptr source) {
  auto flowgraph_lock = gr_lock(to_top_block());
  disconnect(m_source);
  m_source = source;
  connect(m_source, 0, waterfall_sink, 0);

  if (m_udp_sink) {
    connect(m_source, 0, m_udp_sink, 0);
  }
}

void Flowgraph::enable_udp_debug(const std::string &host, int port,
                                 int payload_size, bool eof) {
  if (m_udp_sink) {
    disable_udp_debug();
  }

  auto flowgraph_lock = gr_lock(to_top_block());

  m_udp_sink = gr::blocks::udp_sink::make(sizeof(gr_complex), host, port,
                                          payload_size, eof);
  connect(m_source, 0, m_udp_sink, 0);
}

void Flowgraph::disable_udp_debug() {
  if (m_udp_sink) {
    auto flowgraph_lock = gr_lock(to_top_block());
    disconnect(m_udp_sink);
    m_udp_sink = nullptr;
  }
}
