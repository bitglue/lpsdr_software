#ifndef LPSDR_DISPATCHER_SINK_H
#define LPSDR_DISPATCHER_SINK_H

#include <boost/lockfree/spsc_queue.hpp>
#include <glibmm/dispatcher.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/sync_block.h>

class dispatcher_sink : virtual public gr::sync_block {
public:
  typedef boost::shared_ptr<dispatcher_sink> sptr;
  typedef boost::shared_ptr<float> waterfall_item;
  typedef sigc::signal<void, float *, unsigned> signal_fft_done_t;

  signal_fft_done_t fft_done;

  /*!
   * \brief Return a shared_ptr to a new instance of lpsdr::dispatcher_sink.
   *
   * To avoid accidental use of raw pointers, dispatcher_sink's constructor is
   * in a private implementation class. dispatcher_sink::make is the public
   * interface for creating new instances.
   */
  static sptr make();
};

class dispatcher_sink_impl : public dispatcher_sink {
public:
  dispatcher_sink_impl();
  ~dispatcher_sink_impl();

  int work(int noutput_items, gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

protected:
  gr::fft::fft_complex fft;
  std::vector<float> window;
  int items_in_buffer;

  // buffers are popped from free_buffers and pushed into full_buffers as FFTs
  // are calculated (in the GNU Radio thread), then back in the other direction
  // after being delivered to the fft_done signal (in the GTK main thread).
  boost::lockfree::spsc_queue<waterfall_item, boost::lockfree::capacity<4>>
      free_buffers;
  boost::lockfree::spsc_queue<waterfall_item, boost::lockfree::capacity<4>>
      full_buffers;

  void build_window();
  void execute_fft();

  // Signals that some FFTs are ready in full_buffers.
  Glib::Dispatcher items_available;

  // consume all the full_buffers, emitting them on the fft_done signal.
  //
  // Called in the GTK main thread.
  void dispatch_signals();
};

#endif
