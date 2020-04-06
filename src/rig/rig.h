#ifndef LPSDR_RIG_RIG_H
#define LPSDR_RIG_RIG_H

#include <gnuradio/hier_block2.h>

class LPSDRRig {
public:
  virtual ~LPSDRRig(){};

  virtual gr::basic_block_sptr source() = 0;
  virtual void set_freq(double) = 0;
  virtual double get_freq() = 0;
  virtual void set_dly(int){};
};

#endif
