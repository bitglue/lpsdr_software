#ifndef LPSDR_DEMOD_H
#define LPSDR_DEMOD_H

#include <gnuradio/hier_block2.h>

class Mode {
public:
  typedef std::shared_ptr<Mode> sptr;

  virtual ~Mode(){};
  virtual gr::basic_block_sptr demod() = 0;
};

#endif
