#include "jack.h"
#include "config.h"

Jack::Jack() {
  jack_status_t status;

  client = jack_client_open(PACKAGE_NAME, JackNullOption, &status, NULL);
}

Jack::~Jack() {
  if (client) {
    jack_client_close(client);
    client = NULL;
  }
}

int Jack::jack_process(jack_nframes_t nframes, void *arg) { return 0; }
