#ifndef INCLUDED_LPSDR_DISPATCHER_SINK_IMPL_H
#define INCLUDED_LPSDR_DISPATCHER_SINK_IMPL_H

#include "waterfall.h"
#include <gnuradio/fft/fft.h>
#include <gnuradio/sync_block.h>

class dispatcher_sink : virtual public gr::sync_block {
public:
  typedef boost::shared_ptr<dispatcher_sink> sptr;

  /*!
   * \brief Return a shared_ptr to a new instance of lpsdr::dispatcher_sink.
   *
   * To avoid accidental use of raw pointers, dispatcher_sink's constructor is
   * in a private implementation class. dispatcher_sink::make is the public
   * interface for creating new instances.
   */
  static sptr make(Waterfall &waterfall);
};

class dispatcher_sink_impl : public dispatcher_sink {
public:
  dispatcher_sink_impl(Waterfall &waterfall);
  ~dispatcher_sink_impl();

  int work(int noutput_items, gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

protected:
  Waterfall &waterfall;
  gr::fft::fft_complex fft;
  int items_in_buffer;
};

#endif
