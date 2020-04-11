#include "mode/ssb.h"
#include <gnuradio/filter/firdes.h>
#include <gtkmm/button.h>

// TODO: don't hardcode
const float sample_rate = 48000;

SSBDemod::sptr SSBDemod::make() {
  return gnuradio::get_initial_sptr(new SSBDemod());
}

SSBDemod::SSBDemod()
    : gr::hier_block2("SSBDemod",
                      gr::io_signature::make(1, 1, sizeof(gr_complex)),
                      gr::io_signature::make(1, 1, sizeof(float))) {

  auto taps = make_taps(sample_rate, 300, 3000, 300);
  m_filter = gr::filter::fft_filter_ccc::make(1, taps);
  m_to_float = gr::blocks::complex_to_float::make();
  connect(self(), 0, m_filter, 0);
  connect(m_filter, 0, m_to_float, 0);
  connect(m_to_float, 0, self(), 0);
}

SSBDemod::~SSBDemod() {}

std::vector<gr_complex> SSBDemod::make_taps(double samp_rate, double low_cutoff,
                                            double high_cutoff,
                                            double transition) {
  return gr::filter::firdes::complex_band_pass(1, samp_rate, low_cutoff,
                                               high_cutoff, transition);
}

void SSBDemod::set_taps(double samp_rate, double low_cutoff, double high_cutoff,
                        double transition) {
  auto taps = make_taps(samp_rate, low_cutoff, high_cutoff, transition);
  m_filter->set_taps(taps);
}

SSB::sptr SSB::make(bool lower_sideband) {
  return SSB::sptr(new SSB(lower_sideband));
}

SSB::SSB(bool lower_sideband)
    : m_lower(Gtk::Adjustment::create(300, 0, 500)),
      m_upper(Gtk::Adjustment::create(3000, 10, 10000)),
      m_transition(Gtk::Adjustment::create(300, 100, 1000)),
      m_lower_sideband(lower_sideband) {
  m_builder = Gtk::Builder::create_from_file("src/mode/ssb.glade");

  m_builder->get_widget("lower_spin", m_lower_spin);
  m_builder->get_widget("upper_spin", m_upper_spin);
  m_builder->get_widget("transition_spin", m_transition_spin);

  m_lower_spin->set_adjustment(m_lower);
  m_upper_spin->set_adjustment(m_upper);
  m_transition_spin->set_adjustment(m_transition);

  m_demod = SSBDemod::make();

  m_lower->signal_value_changed().connect(
      sigc::mem_fun(*this, &SSB::on_filter_changed));
  m_upper->signal_value_changed().connect(
      sigc::mem_fun(*this, &SSB::on_filter_changed));
  m_transition->signal_value_changed().connect(
      sigc::mem_fun(*this, &SSB::on_filter_changed));
  on_filter_changed();
}

gr::basic_block_sptr SSB::demod() { return m_demod; }

Gtk::Widget &SSB::settings_widget() {
  Gtk::Widget *w;
  m_builder->get_widget("settings_widget", w);
  return *w;
}

void SSB::on_filter_changed() {
  double low_cutoff, high_cutoff;
  auto transition = m_transition->get_value();
  m_lower->set_upper(m_upper->get_value() - 1);
  m_upper->set_lower(m_lower->get_value() + 1);
  if (m_lower_sideband) {
    low_cutoff = m_lower->get_value();
    high_cutoff = m_upper->get_value();
  } else {
    high_cutoff = -m_lower->get_value();
    low_cutoff = -m_upper->get_value();
  }

  m_demod->set_taps(sample_rate, low_cutoff, high_cutoff, transition);
}
