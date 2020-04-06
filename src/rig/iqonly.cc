#include "iqonly.h"

IQOnly::IQOnly() { m_source = iq_audio_source::make(); }

gr::basic_block_sptr IQOnly::source() { return m_source; }
void IQOnly::set_dly(int d) { m_source->set_dly(d); }
void IQOnly::set_freq(double freq) { m_freq = freq; }
double IQOnly::get_freq() { return m_freq; }
