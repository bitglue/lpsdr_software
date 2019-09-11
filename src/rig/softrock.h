#ifndef LPSDR_RIG_SOFTROCK_H
#define LPSDR_RIG_SOFTROCK_H

#include "rig.h"
#include <hamlib/rigclass.h>

class Softrock : public LPSDRRig {
public:
  Softrock();
  gr::basic_block_sptr source();
  void set_freq(double);

protected:
  Rig rig;
};

#endif
