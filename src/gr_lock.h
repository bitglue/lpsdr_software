#ifndef LPSDR_GR_LOCK_H
#define LPSDR_GR_LOCK_H

#include <gnuradio/top_block.h>

class gr_lock {
public:
  gr_lock(gr::top_block_sptr top_block) : m_top_block(top_block) {
    m_top_block->lock();
  };
  ~gr_lock() { m_top_block->unlock(); };

protected:
  gr::top_block_sptr m_top_block;
};

#endif
