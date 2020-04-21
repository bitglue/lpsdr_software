#ifndef LPSDR_WATERFALL_H
#define LPSDR_WATERFALL_H

#include <cairomm/surface.h>
#include <glibmm/dispatcher.h>
#include <glibmm/property.h>
#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturepan.h>
#include <gtkmm/gestureswipe.h>

class Waterfall : public Gtk::DrawingArea {
public:
  Waterfall();
  virtual ~Waterfall();

  // called from GNU Radio thread
  void add_fft(float *fft, unsigned size);

  static const unsigned fft_size = 4096;
  void set_sensitivity(float);
  void set_range(float);
  void set_bandwidth(unsigned bw);

  float bottom_freq() const { return center_freq() - bandwidth() / 2; };
  float top_freq() const { return center_freq() + bandwidth() / 2; };
  float center_freq() const { return m_center.get_value(); };
  float bandwidth() const { return m_bandwidth.get_value(); };

  Glib::PropertyProxy<float> property_px_per_hz() {
    return m_px_per_hz.get_proxy();
  }
  Glib::PropertyProxy<float> property_center() { return m_center.get_proxy(); }
  Glib::PropertyProxy<float> property_bandwidth() {
    return m_bandwidth.get_proxy();
  }

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
  void draw_background(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_scale(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_tick(const Cairo::RefPtr<Cairo::Context> &cr, float freq);

  void on_gesture_begin(GdkEventSequence *seq);
  void on_gesture_pan(Gtk::PanDirection direction, double offset);
  void on_gesture_swipe(double velocity_x, double velocity_y);
  void on_fft_added();
  void on_freq_changed();

  void update_kinematics();
  bool on_tick(const Glib::RefPtr<Gdk::FrameClock> &frame_clock);

  float px_to_hz(float px) {
    return bandwidth() / get_allocation().get_width() * px;
  };

  Glib::Property<float> m_px_per_hz;
  Glib::Property<float> m_center;
  Glib::Property<float> m_bandwidth;

  float gesture_begin_center;
  float swipe_velocity;
  float fft_min;
  float fft_scale;

  Glib::RefPtr<Gtk::GesturePan> gesture_pan;
  Glib::RefPtr<Gtk::GestureSwipe> gesture_swipe;

  const int background_height = 256;
  int background_stride;
  const Cairo::Format background_format = Cairo::Format::FORMAT_RGB24;
  Cairo::RefPtr<Cairo::ImageSurface> background;
  unsigned char *background_data;
  Glib::Dispatcher on_add_fft_dispatcher;

  virtual void on_size_allocate(Gtk::Allocation &allocation) override;
};

#endif
