#include "mode/ssb.h"
#include <gnuradio/filter/firdes.h>
#include <gtkmm/button.h>

SSBDemod::sptr SSBDemod::make() {
  return gnuradio::get_initial_sptr(new SSBDemod());
}

SSBDemod::SSBDemod()
    : gr::hier_block2("SSBDemod",
                      gr::io_signature::make(1, 1, sizeof(gr_complex)),
                      gr::io_signature::make(1, 1, sizeof(float))) {
  auto taps = std::vector<gr_complex>{1};

  m_rotator = gr::blocks::rotator_cc::make();
  m_filter = gr::filter::fft_filter_ccc::make(1, taps);
  m_to_float = gr::blocks::complex_to_float::make();

  connect(self(), 0, m_rotator, 0);
  connect(m_rotator, 0, m_filter, 0);
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

SSB::sptr SSB::make(unsigned sample_rate, bool lower_sideband) {
  return SSB::sptr(new SSB(sample_rate, lower_sideband));
}

SSB::SSB(unsigned sample_rate, bool lower_sideband)
    : Glib::ObjectBase(typeid(SSB)), m_carrier_offset(*this, "carrier-offset"),
      m_lower(Gtk::Adjustment::create(300, 0, 500)),
      m_upper(Gtk::Adjustment::create(3000, 10, 10000)),
      m_transition(Gtk::Adjustment::create(300, 100, 1000)),
      m_lower_sideband(lower_sideband), m_sample_rate(sample_rate) {
  if (lower_sideband) {
    m_lower->set_lower(-10000);
    m_lower->set_upper(-10);
    m_lower->set_value(-3000);
    m_upper->set_lower(-1000);
    m_upper->set_upper(-100);
    m_upper->set_value(-300);
  }
  m_builder =
      Gtk::Builder::create_from_resource("/com/bitglue/LPSDR/mode/ssb.glade");

  m_builder->get_widget("lower_scale", m_lower_scale);
  m_builder->get_widget("upper_scale", m_upper_scale);
  m_builder->get_widget("transition_scale", m_transition_scale);

  m_lower_scale->set_adjustment(m_lower);
  m_upper_scale->set_adjustment(m_upper);
  m_transition_scale->set_adjustment(m_transition);

  m_demod = SSBDemod::make();

  m_lower->signal_value_changed().connect(
      sigc::mem_fun(*this, &SSB::on_filter_changed));
  m_upper->signal_value_changed().connect(
      sigc::mem_fun(*this, &SSB::on_filter_changed));
  m_transition->signal_value_changed().connect(
      sigc::mem_fun(*this, &SSB::on_filter_changed));
  on_filter_changed();
  property_carrier_offset().signal_changed().connect(
      sigc::mem_fun(*this, &SSB::on_carrier_offset_changed));
}

gr::basic_block_sptr SSB::demod() { return m_demod; }

Gtk::Widget &SSB::settings_widget() {
  Gtk::Widget *w;
  m_builder->get_widget("settings_widget", w);
  return *w;
}

void SSB::on_filter_changed() {
  auto transition = m_transition->get_value(),
       low_cutoff = m_lower->get_value(), high_cutoff = m_upper->get_value();

  m_lower->set_upper(high_cutoff - 1);
  m_upper->set_lower(low_cutoff + 1);

  m_demod->set_taps(m_sample_rate, low_cutoff, high_cutoff, transition);
}

void SSB::on_carrier_offset_changed() {
  m_demod->set_phase_inc(-carrier_offset() / m_sample_rate * 2 * M_PI);
}

Glib::PropertyProxy<double> SSB::property_lower_cutoff() {
  return m_lower->property_value();
}
Glib::PropertyProxy<double> SSB::property_upper_cutoff() {
  return m_upper->property_value();
}
Glib::PropertyProxy<double> SSB::property_carrier_offset() {
  return m_carrier_offset.get_proxy();
}
