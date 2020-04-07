#ifndef LPSDR_RIG_IQONLY_H
#define LPSDR_RIG_IQONLY_H

#include "iq_audio_source.h"
#include "rig.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/spinbutton.h>

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
  Gtk::SpinButton *m_magnitude_spin, *m_phase_spin, *m_delay_spin;
  Glib::RefPtr<Gtk::Adjustment> m_magnitude_adjustment, m_phase_adjustment,
      m_delay_adjustment;
  void on_delay_changed();
};

#endif
