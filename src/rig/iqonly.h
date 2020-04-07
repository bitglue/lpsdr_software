#ifndef LPSDR_RIG_IQONLY_H
#define LPSDR_RIG_IQONLY_H

#include "iq_audio_source.h"
#include "rig.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/scale.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/togglebutton.h>

class IQOnly : public LPSDRRig {
public:
  IQOnly();
  gr::basic_block_sptr source();
  void set_freq(double);
  double get_freq();
  void show_settings_window();

protected:
  double m_freq;
  iq_audio_source::sptr m_source;
  Gtk::Window *m_settings_window;
  Gtk::SpinButton *m_delay_spin;
  Glib::RefPtr<Gtk::Adjustment> m_magnitude_adjustment, m_phase_adjustment,
      m_delay_adjustment;
  Gtk::Scale *m_magnitude_scale, *m_phase_scale;
  Gtk::ToggleButton *m_iq_auto_button;
  sigc::connection iq_bal_poller;

  void on_delay_changed();
  void on_iq_auto_toggled();
  void on_iq_bal_changed();
  bool on_timeout();
  bool iq_auto_enabled() { return m_iq_auto_button->get_active(); }
};

#endif
