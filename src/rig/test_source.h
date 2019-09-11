#ifndef LPSDR_RIG_TEST_SOURCE_H
#define LPSDR_RIG_TEST_SOURCE_H

#include <gnuradio/analog/noise_source_c.h>
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
};

class test_source_impl : public test_source {
public:
  test_source_impl();
  ~test_source_impl();

protected:
  gr::analog::noise_source_c::sptr noise_source;
  gr::blocks::throttle::sptr throttle;
};

#endif
