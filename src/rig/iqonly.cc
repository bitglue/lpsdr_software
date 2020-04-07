#include "iqonly.h"
#include <glibmm/main.h>
#include <gtkmm/builder.h>

IQOnly::IQOnly()
    : m_magnitude_adjustment(Gtk::Adjustment::create(0, -0.5, 0.5, 0.000001)),
      m_phase_adjustment(Gtk::Adjustment::create(0, -0.5, 0.5, 0.000001)),
      m_delay_adjustment(Gtk::Adjustment::create(1, -10, 10, 1)) {
  m_source = iq_audio_source::make();

  auto builder = Gtk::Builder::create_from_file("src/iq_audio.glade");
  builder->get_widget("settings_window", m_settings_window);
  builder->get_widget("delay_spin", m_delay_spin);
  builder->get_widget("magnitude_scale", m_magnitude_scale);
  builder->get_widget("phase_scale", m_phase_scale);
  builder->get_widget("iq_auto_button", m_iq_auto_button);
  builder->get_widget("device_entry", m_device_entry);

  m_magnitude_scale->set_adjustment(m_magnitude_adjustment);
  m_magnitude_scale->set_increments(0.0001, 0.0001);
  m_magnitude_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &IQOnly::on_iq_bal_changed));

  m_phase_scale->set_adjustment(m_phase_adjustment);
  m_phase_scale->set_increments(0.0001, 0.0001);
  m_phase_adjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &IQOnly::on_iq_bal_changed));

  m_delay_spin->set_adjustment(m_delay_adjustment);
  m_delay_spin->signal_value_changed().connect(
      sigc::mem_fun(*this, &IQOnly::on_delay_changed));

  m_iq_auto_button->signal_toggled().connect(
      sigc::mem_fun(*this, &IQOnly::on_iq_auto_toggled));

  m_device_entry->signal_activate().connect(
      sigc::mem_fun(*this, &IQOnly::on_device_changed));

  on_delay_changed();
}

gr::basic_block_sptr IQOnly::source() { return m_source; }
void IQOnly::set_freq(double freq) { m_freq = freq; }
double IQOnly::get_freq() { return m_freq; }

void IQOnly::show_settings_window() { m_settings_window->show(); }

void IQOnly::on_delay_changed() {
  m_source->set_dly(m_delay_adjustment->get_value());
}

void IQOnly::on_iq_auto_toggled() {
  auto auto_enabled = m_iq_auto_button->get_active();
  m_magnitude_scale->set_sensitive(not auto_enabled);
  m_phase_scale->set_sensitive(not auto_enabled);
  m_source->set_auto_iq_correction(auto_enabled);

  if (auto_enabled && iq_bal_poller.empty()) {
    iq_bal_poller = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &IQOnly::on_timeout), 200);
  } else if (not auto_enabled) {
    iq_bal_poller.disconnect();
  }
}

void IQOnly::on_iq_bal_changed() {
  if (not iq_auto_enabled()) {
    m_source->set_iq_correction(m_magnitude_adjustment->get_value(),
                                m_phase_adjustment->get_value());
  }
}

bool IQOnly::on_timeout() {
  if (iq_auto_enabled()) {
    m_magnitude_adjustment->set_value(m_source->iq_fix_mag());
    m_phase_adjustment->set_value(m_source->iq_fix_phase());
    return true;
  } else {
    return false;
  }
}

void IQOnly::on_device_changed() {
  auto old_device = m_source->device_name();
  auto new_device = m_device_entry->get_text();

  if (old_device == new_device)
    return;
  try {
    m_source->set_audio_device(new_device);
  } catch (std::runtime_error &e) {
    std::cout << e.what() << "\n" << m_source->device_name() << "\n";
    // TODO: some better error handling would be nice here.
    m_device_entry->set_text(old_device);
  }
}
