#ifndef LPSDR_MODE_SSB_H
#define LPSDR_MODE_SSB_H

#include "mode/mode.h"
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/filter/fft_filter_ccc.h>
#include <gnuradio/hier_block2.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/builder.h>
#include <gtkmm/scale.h>

class SSBDemod : virtual public gr::hier_block2 {
public:
  typedef boost::shared_ptr<SSBDemod> sptr;
  static sptr make();
  void set_taps(double samp_rate, double low_cutoff, double high_cutoff,
                double transition);

private:
  SSBDemod();
  virtual ~SSBDemod();
  static std::vector<gr_complex> make_taps(double samp_rate, double low_cutoff,
                                           double high_cutoff,
                                           double transition);
  boost::shared_ptr<gr::filter::fft_filter_ccc> m_filter;
  gr::blocks::complex_to_float::sptr m_to_float;
};

class SSB : virtual public Mode {
public:
  typedef std::shared_ptr<SSB> sptr;
  static sptr make(unsigned sample_rate, bool lower_sideband);
  gr::basic_block_sptr demod();
  Gtk::Widget &settings_widget();

private:
  SSB(unsigned sample_rate, bool lower_sideband);
  Glib::RefPtr<Gtk::Builder> m_builder;
  Glib::RefPtr<Gtk::Adjustment> m_lower, m_upper, m_transition;
  Gtk::Scale *m_lower_scale, *m_upper_scale, *m_transition_scale;
  SSBDemod::sptr m_demod;
  bool m_lower_sideband;
  const unsigned m_sample_rate;

  void on_filter_changed();
};

#endif
