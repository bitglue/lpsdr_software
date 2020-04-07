#ifndef LPSDR_GR_LOCK_H
#define LPSDR_GR_LOCK_H

#include <gnuradio/hier_block2.h>

class gr_lock {
public:
  gr_lock(gr::hier_block2_sptr hier_block2) : m_hier_block2(hier_block2) {
    m_hier_block2->lock();
  };
  ~gr_lock() { m_hier_block2->unlock(); };

protected:
  gr::hier_block2_sptr m_hier_block2;
};

#endif
