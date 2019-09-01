#ifndef LPSDR_WATERFALL_H
#define LPSDR_WATERFALL_H

#include <cairomm/surface.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturepan.h>
#include <gtkmm/gestureswipe.h>
#include <gtkmm/gesturezoom.h>

class Waterfall : public Gtk::DrawingArea {
public:
  Waterfall();
  virtual ~Waterfall();

  // called from GNU Radio thread
  void add_fft(float *fft);
  static const int fft_size = 4096;

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
  void draw_background(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_scale(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_tick(const Cairo::RefPtr<Cairo::Context> &cr, float freq);

  void on_gesture_begin(GdkEventSequence *seq);
  void on_gesture_zoom(double scale);
  void on_gesture_pan(Gtk::PanDirection direction, double offset);
  void on_gesture_swipe(double velocity_x, double velocity_y);
  void on_fft_added();

  void update_kinematics();
  bool on_tick(const Glib::RefPtr<Gdk::FrameClock> &frame_clock);

  float bottom_freq;
  float top_freq;
  float swipe_velocity;
  float gesture_begin_bottom_freq;
  float gesture_begin_top_freq;

  Glib::RefPtr<Gtk::GestureZoom> gesture_zoom;
  Glib::RefPtr<Gtk::GesturePan> gesture_pan;
  Glib::RefPtr<Gtk::GestureSwipe> gesture_swipe;

  const int background_height = 256;
  int background_stride;
  const Cairo::Format background_format = Cairo::Format::FORMAT_RGB24;
  Cairo::RefPtr<Cairo::ImageSurface> background;
  unsigned char *background_data;
  Glib::Dispatcher on_add_fft_dispatcher;
};

#endif
