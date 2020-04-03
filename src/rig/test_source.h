#ifndef LPSDR_RIG_TEST_SOURCE_H
#define LPSDR_RIG_TEST_SOURCE_H

#include <gnuradio/analog/noise_source_c.h>
#include <gnuradio/analog/sig_source_c.h>
#include <gnuradio/blocks/add_cc.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/hier_block2.h>

class test_source : virtual public gr::hier_block2 {
public:
  typedef boost::shared_ptr<test_source> sptr;

  /*!
   * \brief Return a shared_ptr to a new instance of lpsdr::dispatcher_sink.
   *
   * To avoid accidental use of raw pointers, dispatcher_sink's constructor is
   * in a private implementation class. dispatcher_sink::make is the public
   * interface for creating new instances.
   */
  static sptr make();

  virtual void set_freq(double) = 0;
};

class test_source_impl : public test_source {
public:
  test_source_impl();
  ~test_source_impl();

  void set_freq(double);

protected:
  gr::analog::noise_source_c::sptr noise_source;
  gr::analog::sig_source_c::sptr sig_source;
  gr::blocks::throttle::sptr throttle;
  gr::blocks::add_cc::sptr add;
};

#endif
