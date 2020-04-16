#ifndef LPSDR_RIG_SOFTROCK_H
#define LPSDR_RIG_SOFTROCK_H

#include "iqonly.h"
#include <hamlib/rigclass.h>

class Softrock : public IQOnly {
public:
  Softrock(unsigned sample_rate);
  void set_freq(double);
  double get_freq();

protected:
  Rig rig;
};

#endif
