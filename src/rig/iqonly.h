#ifndef LPSDR_RIG_IQONLY_H
#define LPSDR_RIG_IQONLY_H

#include "iq_audio_source.h"
#include "rig.h"

class IQOnly : public LPSDRRig {
public:
  IQOnly();
  gr::basic_block_sptr source();
  void set_freq(double);
  void set_dly(int);
  double get_freq();

protected:
  double m_freq;
  iq_audio_source::sptr m_source;
};

#endif
