#include "mode/wspr.h"
#include <cstdio>
#include <glibmm/main.h>
#include <gnuradio/filter/firdes.h>

// wsprd ignores the WAV header: input must be 12 kHz, 16 bit
const unsigned wsprd_sample_rate = 12000;
const unsigned wsprd_bit_depth = 16;

// TODO: don't hardcode
const unsigned sample_rate = 48000;

WSPR::sptr WSPR::make() { return sptr(new WSPR()); }

WSPR::WSPR() : m_interval(std::chrono::seconds(120)) {
  m_demod = WSPRDemod::make();

  m_builder =
      Gtk::Builder::create_from_resource("/com/bitglue/LPSDR/mode/wspr.glade");
  m_builder->get_widget("progress_bar", m_progress_bar);

  m_progress_timer_conn = Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &WSPR::on_update_progress), 250);

  // this will start the timers running
  on_finish_recording();
}

WSPR::~WSPR() {
  m_progress_timer_conn.disconnect();
  m_interval_timer_conn.disconnect();
}

Gtk::Widget &WSPR::settings_widget() {
  Gtk::Widget *w;
  m_builder->get_widget("settings_widget", w);
  return *w;
}

bool WSPR::on_update_progress() {
  m_progress_bar->set_fraction(m_interval.fraction());
  return true;
}

bool WSPR::on_start_recording() {
  m_last_file = filename_for_interval(m_interval.number());
  std::cout << "start recording " << m_last_file << "\n";
  m_demod->open(m_last_file.c_str());

  // The transmission is 110.6 seconds long, and starts nominally one second
  // into the interval, so should be done by 111.6 seconds. But we opt to copy
  // the behavior of WSJT-X here and record a few seconds more to accomodate
  // some clock skew, while still leaving 6 seconds to decode before the next
  // interval.
  m_interval_timer_conn = Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &WSPR::on_finish_recording), 114000);
  return false;
}

bool WSPR::on_finish_recording() {
  if (m_last_file.empty()) {
    std::cout << "no recording to finish\n";
  } else {
    std::cout << "finish recording " << m_last_file << "\n";
  }

  auto ms_to_start = std::chrono::duration_cast<std::chrono::milliseconds>(
                         m_interval.until_next())
                         .count();
  m_interval_timer_conn = Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &WSPR::on_start_recording), ms_to_start);
  return false;
}

const std::string WSPR::wav_dir() { return "/tmp/lpsdr"; }

const std::string WSPR::filename_for_interval(unsigned interval) {
  char buf[512];
  snprintf(buf, 512, "%s/lpsdr_wspr_%i.wav", wav_dir().c_str(), interval);
  return buf;
}

// /usr/bin/wsprd -C 5000 -o 4 -a /home/indigo/.local/share/WSJT-X -f 14.095600
// /home/indigo/.local/share/WSJT-X/save/200412_0426.wav
//
// indigo@beast:/tmp/WSJT-X$ file
// /home/indigo/.local/share/WSJT-X/save/200412_0426.wav
// /home/indigo/.local/share/WSJT-X/save/200412_0426.wav: RIFF (little-endian)
// data, WAVE audio, Microsoft PCM, 16 bit, mono 12000 Hz
//
// wspr is centered at 1.5 kHz in the file, +/- 100 Hz, or +/- 150 with -w
// option

std::vector<gr_complex> WSPRDemod::design_filter(unsigned interpolation,
                                                 unsigned decimation) {
  const float beta = 7.0;

  const float
      // wsprd looks at a band centered at 1.5 kHz
      wspr_center = 1500,

      // with the -w option, it looks +/- 150 Hz from the center
      wspr_bw = 150,

      // the transition band will alias into the low frequencies, but stop
      // before the bottom edge of the WSPR band at 1350 Hz. Logically, it
      // should be possible to make this as large as 1350*2, but testing
      // indicates there's more attenuation to be had if we make it a bit
      // smaller.
      transition_width = 2150,

      // the firdes functions define the cutoff as the -6 dB point, which is
      // halfway through the transition band.
      upper_cutoff = wspr_center + wspr_bw + transition_width / 2,
      lower_cutoff = wspr_center - wspr_bw - transition_width / 2;

  auto taps = gr::filter::firdes::complex_band_pass(
      interpolation,                  // gain
      decimation * wsprd_sample_rate, // sampling freq
      lower_cutoff,                   // low cutoff
      upper_cutoff,                   // high cutoff
      transition_width,               // transition width
      gr::filter::firdes::WIN_KAISER, beta);

  return taps;
}

gr::basic_block_sptr WSPR::demod() { return m_demod; }

WSPRDemod::sptr WSPRDemod::make() { return sptr(new WSPRDemod()); }

WSPRDemod::WSPRDemod()
    : gr::hier_block2("WSPRDemod",
                      gr::io_signature::make(1, 1, sizeof(gr_complex)),
                      gr::io_signature::make(0, 0, 0)) {

  unsigned interpolation = wsprd_sample_rate;
  unsigned decimation = sample_rate;
  unsigned gcd = std::__gcd(interpolation, decimation);
  interpolation /= gcd;
  decimation /= gcd;

  auto taps = design_filter(interpolation, decimation);

  m_resampler = gr::filter::rational_resampler_base_ccc::make(interpolation,
                                                              decimation, taps);
  m_to_real = gr::blocks::complex_to_real::make();
  m_wav_sink = gr::blocks::wavfile_sink::make("/dev/null", 1, wsprd_sample_rate,
                                              wsprd_bit_depth);

  connect(self(), 0, m_resampler, 0);
  connect(m_resampler, 0, m_to_real, 0);
  connect(m_to_real, 0, m_wav_sink, 0);
}

bool WSPRDemod::open(const char *filename) {
  return m_wav_sink->open(filename);
}
