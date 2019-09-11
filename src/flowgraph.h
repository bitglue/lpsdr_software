#ifndef LIBSDR_FLOWGRAPH_H
#define LIBSDR_FLOWGRAPH_H

#include "dispatcher_sink.h"
#include <gnuradio/hier_block2.h>
#include <gnuradio/top_block.h>

class Flowgraph : public gr::top_block {
public:
  typedef boost::shared_ptr<Flowgraph> sptr;

  Flowgraph(gr::basic_block_sptr source);
  virtual ~Flowgraph();
  dispatcher_sink::signal_fft_done_t signal_fft_done();

  static sptr make(gr::basic_block_sptr source);

protected:
  gr::basic_block_sptr m_source;
  dispatcher_sink::sptr waterfall_sink;
};

#endif
