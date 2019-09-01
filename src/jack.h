#ifndef LPSDR_JACK_H
#define LPSDR_JACK_H

#include <jack/jack.h>

class Jack {
public:
  Jack();
  virtual ~Jack();

protected:
  jack_client_t *client;
  int jack_process(jack_nframes_t nframes, void *arg);
};

#endif
