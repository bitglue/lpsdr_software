#include "dispatcher_sink.h"
#include <assert.h>
#include <gnuradio/io_signature.h>
#include <string.h>
#include <volk/volk.h>

static const int fft_size = 4096;

dispatcher_sink::sptr dispatcher_sink::make() {
  return gnuradio::get_initial_sptr(new dispatcher_sink_impl());
}

dispatcher_sink_impl::dispatcher_sink_impl()
    : gr::sync_block("dispatcher_sink",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      fft(fft_size), items_in_buffer(0) {
  while (free_buffers.write_available()) {
    auto buffer = boost::shared_ptr<float>(new float[fft_size]);
    free_buffers.push(buffer);
  }

  items_available.connect(
      sigc::mem_fun(*this, &dispatcher_sink_impl::dispatch_signals));
}

dispatcher_sink_impl::~dispatcher_sink_impl() {}

int dispatcher_sink_impl::work(int noutput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items) {
  const gr_complex *in = (const gr_complex *)input_items[0];

  int produced_items = 0;

  while (produced_items < noutput_items) {
    int to_copy = noutput_items - produced_items;
    if (to_copy + items_in_buffer > fft_size) {
      to_copy = fft_size - items_in_buffer;
    }

    assert(to_copy <= noutput_items);
    assert(to_copy + items_in_buffer <= fft_size);

    memcpy(fft.get_inbuf() + items_in_buffer, in, to_copy * sizeof(gr_complex));
    produced_items += to_copy;
    in += to_copy;
    items_in_buffer += to_copy;

    assert(items_in_buffer <= fft_size);

    if (items_in_buffer == fft_size) {
      execute_fft();
      items_in_buffer = 0;
    }
  }

  assert(noutput_items == produced_items);
  return noutput_items;
}

void dispatcher_sink_impl::execute_fft() {
  fft.execute();
  waterfall_item magnitudes;
  if (free_buffers.pop(magnitudes)) {
    volk_32fc_s32f_x2_power_spectral_density_32f(
        magnitudes.get(), fft.get_outbuf(), fft_size, 1.0, fft_size);
    if (full_buffers.push(magnitudes)) {
      items_available.emit();
    }
  }
}

void dispatcher_sink_impl::dispatch_signals() {
  waterfall_item magnitudes;
  while (full_buffers.pop(magnitudes)) {
    fft_done.emit(magnitudes.get(), fft_size);
    free_buffers.push(magnitudes);
  }
}
