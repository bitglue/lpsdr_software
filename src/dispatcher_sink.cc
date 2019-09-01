#include "dispatcher_sink.h"
#include <assert.h>
#include <gnuradio/io_signature.h>
#include <string.h>
#include <volk/volk.h>

dispatcher_sink::sptr dispatcher_sink::make(Waterfall &waterfall) {
  return gnuradio::get_initial_sptr(new dispatcher_sink_impl(waterfall));
}

dispatcher_sink_impl::dispatcher_sink_impl(Waterfall &waterfall)
    : gr::sync_block("dispatcher_sink",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      waterfall(waterfall), fft(waterfall.fft_size), items_in_buffer(0) {}

dispatcher_sink_impl::~dispatcher_sink_impl() {}

int dispatcher_sink_impl::work(int noutput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items) {
  const gr_complex *in = (const gr_complex *)input_items[0];

  int produced_items = 0;

  while (produced_items < noutput_items) {
    int to_copy = noutput_items - produced_items;
    if (to_copy + items_in_buffer > waterfall.fft_size) {
      to_copy = waterfall.fft_size - items_in_buffer;
    }

    assert(to_copy <= noutput_items);
    assert(to_copy + items_in_buffer <= waterfall.fft_size);

    memcpy(fft.get_inbuf() + items_in_buffer, in, to_copy * sizeof(gr_complex));
    produced_items += to_copy;
    in += to_copy;
    items_in_buffer += to_copy;

    assert(items_in_buffer <= waterfall.fft_size);

    if (items_in_buffer == waterfall.fft_size) {
      fft.execute();
      float magnitudes[waterfall.fft_size];
      volk_32fc_s32f_x2_power_spectral_density_32f(magnitudes, fft.get_outbuf(),
                                                   waterfall.fft_size, 1.0,
                                                   waterfall.fft_size);
      waterfall.add_fft(magnitudes);
      items_in_buffer = 0;
    }
  }

  assert(noutput_items == produced_items);
  return noutput_items;
}
