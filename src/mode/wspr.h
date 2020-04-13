#ifndef LPSDR_MODE_WSPR_H
#define LPSDR_MODE_WSPR_H

#include "jtinterval.h"
#include "mode/mode.h"
#include <chrono>
#include <glibmm.h>
#include <gnuradio/blocks/complex_to_real.h>
#include <gnuradio/blocks/wavfile_sink.h>
#include <gnuradio/filter/rational_resampler_base_ccc.h>
#include <gtkmm/builder.h>
#include <gtkmm/progressbar.h>

class WSPRDemod : virtual public gr::hier_block2 {
public:
  typedef boost::shared_ptr<WSPRDemod> sptr;
  static sptr make();
  bool open(const char *filename);

private:
  WSPRDemod();
  gr::filter::rational_resampler_base_ccc::sptr m_resampler;
  gr::blocks::complex_to_real::sptr m_to_real;
  gr::blocks::wavfile_sink::sptr m_wav_sink;

  static std::vector<gr_complex> design_filter(unsigned interpolation,
                                               unsigned decimation);
};

class WSPR : virtual public Mode {
public:
  typedef std::shared_ptr<WSPR> sptr;
  static sptr make();
  gr::basic_block_sptr demod();
  Gtk::Widget &settings_widget();
  ~WSPR();

private:
  JTInterval m_interval;
  WSPRDemod::sptr m_demod;
  Glib::RefPtr<Gtk::Builder> m_builder;
  Gtk::ProgressBar *m_progress_bar;
  sigc::connection m_progress_timer_conn;
  sigc::connection m_interval_timer_conn;
  std::string m_recoding_file;
  std::string m_decoding_file;
  std::string m_wav_dir;
  Glib::RefPtr<Glib::IOChannel> m_wsprd_stdout_channel;

  WSPR();
  std::string wav_dir() const;
  std::string filename_for_interval(unsigned) const;
  bool on_update_progress();
  bool on_start_recording();
  bool on_finish_recording();
  bool on_wsprd_output(Glib::IOCondition io_condition);
};

#endif
