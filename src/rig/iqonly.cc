#include "iqonly.h"
#include <gtkmm/builder.h>

IQOnly::IQOnly()
    : m_magnitude_adjustment(Gtk::Adjustment::create(0, -1, 1, 0.000001)),
      m_phase_adjustment(Gtk::Adjustment::create(0, -1, 1, 0.000001)),
      m_delay_adjustment(Gtk::Adjustment::create(1, -10, 10, 1)) {
  m_source = iq_audio_source::make();

  auto builder = Gtk::Builder::create_from_file("src/iq_audio.glade");
  builder->get_widget("settings_window", m_settings_window);
  builder->get_widget("magnitude_spin", m_magnitude_spin);
  builder->get_widget("phase_spin", m_phase_spin);
  builder->get_widget("delay_spin", m_delay_spin);

  m_magnitude_spin->set_adjustment(m_magnitude_adjustment);
  m_phase_spin->set_adjustment(m_phase_adjustment);
  m_delay_spin->set_adjustment(m_delay_adjustment);

  m_delay_spin->signal_value_changed().connect(
      sigc::mem_fun(*this, &IQOnly::on_delay_changed));

  on_delay_changed();
}

gr::basic_block_sptr IQOnly::source() { return m_source; }
void IQOnly::set_freq(double freq) { m_freq = freq; }
double IQOnly::get_freq() { return m_freq; }

void IQOnly::show_settings_window() { m_settings_window->show(); }

void IQOnly::on_delay_changed() {
  m_source->set_dly(m_delay_adjustment->get_value());
}
