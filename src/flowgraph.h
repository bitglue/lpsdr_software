#ifndef LIBSDR_FLOWGRAPH_H
#define LIBSDR_FLOWGRAPH_H

#include "dispatcher_sink.h"
#include <gnuradio/audio/sink.h>
#include <gnuradio/blocks/udp_sink.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/top_block.h>

class Flowgraph : public gr::top_block {
public:
  typedef boost::shared_ptr<Flowgraph> sptr;

  virtual ~Flowgraph();
  dispatcher_sink::signal_fft_done_t signal_fft_done();

  static sptr make(gr::basic_block_sptr source, unsigned sample_rate);
  void set_source(gr::basic_block_sptr);
  void enable_udp_debug(const std::string &host, int port,
                        int payload_size = 1472, bool eof = true);
  void disable_udp_debug();
  void set_demod(gr::basic_block_sptr);

protected:
  gr::basic_block_sptr m_source, m_demod;
  dispatcher_sink::sptr waterfall_sink;
  gr::blocks::udp_sink::sptr m_udp_sink;
  gr::audio::sink::sptr m_audio_sink;
  const unsigned m_sample_rate;

  Flowgraph(gr::basic_block_sptr source, unsigned sample_rate);
  void make_connections();
};

#endif
