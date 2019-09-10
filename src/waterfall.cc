#include "waterfall.h"
#include <cairomm/context.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <glibmm/main.h>
#include <gtkmm/widget.h>
#include <stdio.h>

Waterfall::Waterfall(Gtk::DrawingArea::BaseObjectType *cobject,
                     const Glib::RefPtr<Gtk::Builder> &builder)
    : Gtk::DrawingArea(cobject), bottom_freq(7e6), top_freq(7.048e6),
      swipe_velocity(0), fft_min(-120), fft_scale(40),
      rig(RIG_MODEL_SI570AVRUSB) {
  // add_tick_callback(sigc::mem_fun(*this, &Waterfall::on_tick));

  // gesture_zoom = Gtk::GestureZoom::create(*this);
  // gesture_zoom->set_propagation_phase(Gtk::PropagationPhase::PHASE_BUBBLE);
  // gesture_zoom->signal_scale_changed().connect(
  //    sigc::mem_fun(*this, &Waterfall::on_gesture_zoom));
  // gesture_zoom->signal_begin().connect(
  //    sigc::mem_fun(*this, &Waterfall::on_gesture_begin));

  gesture_pan =
      Gtk::GesturePan::create(*this, Gtk::Orientation::ORIENTATION_HORIZONTAL);
  gesture_pan->set_propagation_phase(Gtk::PropagationPhase::PHASE_BUBBLE);
  gesture_pan->signal_pan().connect(
      sigc::mem_fun(*this, &Waterfall::on_gesture_pan));
  gesture_pan->signal_begin().connect(
      sigc::mem_fun(*this, &Waterfall::on_gesture_begin));

  gesture_swipe = Gtk::GestureSwipe::create(*this);
  gesture_swipe->set_propagation_phase(Gtk::PropagationPhase::PHASE_BUBBLE);
  gesture_swipe->signal_swipe().connect(
      sigc::mem_fun(*this, &Waterfall::on_gesture_swipe));

  background_stride =
      background->format_stride_for_width(background_format, fft_size);
  background_data = new unsigned char[background_height * background_stride];
  background = background->create(background_data, background_format, fft_size,
                                  background_height, background_stride);

  on_add_fft_dispatcher.connect(sigc::mem_fun(*this, &Waterfall::on_fft_added));

  rig.open();
  on_freq_change();
}

Waterfall::~Waterfall() {
  if (background) {
    background->finish();
  }
  if (background_data) {
    delete background_data;
  }
}

bool Waterfall::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  update_kinematics();
  draw_background(cr);
  draw_scale(cr);

  return true;
}

void Waterfall::draw_background(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  cr->save();
  cr->scale((float)width / (float)fft_size,
            (float)height / (float)background_height);
  cr->set_source(background, 0, 0);
  cairo_pattern_set_filter(cr->get_source()->cobj(), CAIRO_FILTER_BILINEAR);
  cr->paint();
  cr->restore();
}

void Waterfall::draw_scale(const Cairo::RefPtr<Cairo::Context> &cr) {
  const float freq_range = top_freq - bottom_freq;
  float step = 1.0;
  const float label_width = 50;
  const float waterfall_width = get_allocation().get_width();
  const float max_ticks = waterfall_width / label_width;

  while (freq_range / step > max_ticks) {
    step *= 10;
  }
  if (freq_range / step * label_width < waterfall_width / 5) {
    step /= 5;
  } else if (freq_range / step * label_width < waterfall_width / 2) {
    step /= 2;
  }

  for (int i = bottom_freq / step; i * step < top_freq; i++) {
    draw_tick(cr, i * step);
  }
}

void Waterfall::draw_tick(const Cairo::RefPtr<Cairo::Context> &cr, float freq) {
  cr->save();
  float x = (freq - bottom_freq) / (top_freq - bottom_freq) *
            get_allocation().get_width();
  cr->move_to(x, 0);

  const char *suffix = "";

  if (freq >= 1e10) {
    suffix = "G";
    freq /= 1e9;
  } else if (freq >= 1e7) {
    suffix = "M";
    freq /= 1e6;
  } else if (freq >= 1e4) {
    suffix = "k";
    freq /= 1e3;
  }

  char label[20];
  snprintf(label, sizeof(label), "%.4g%s", freq, suffix);

  Pango::FontDescription font;
  font.set_family("Sans");

  cr->set_source_rgb(1.0, 1.0, 1.0);

  auto layout = create_pango_layout(label);
  layout->set_font_description(font);

  layout->show_in_cairo_context(cr);

  cr->restore();
}

void Waterfall::update_kinematics() {
  if (swipe_velocity) {
    float hz = top_freq - bottom_freq;
    float hz_per_px = hz / get_allocation().get_width();
    bottom_freq -= hz_per_px * swipe_velocity;
    top_freq -= hz_per_px * swipe_velocity;
    swipe_velocity *= 0.75;
    if (abs(swipe_velocity) < 1.0) {
      swipe_velocity = 0;
    }
  }
}

bool Waterfall::on_tick(const Glib::RefPtr<Gdk::FrameClock> &frame_clock) {
  update_kinematics();

  float fft[fft_size];
  for (int i = 0; i < fft_size; i++) {
    fft[i] = (float)rand() / (float)RAND_MAX;
  }

  auto win = get_window();
  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                     get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
  return true;
}

void Waterfall::on_gesture_begin(GdkEventSequence *seq) {
  gesture_begin_bottom_freq = bottom_freq;
  gesture_begin_top_freq = top_freq;
  swipe_velocity = 0.0;
}

void Waterfall::on_gesture_zoom(double scale) {
  float center_freq = (gesture_begin_top_freq + gesture_begin_bottom_freq) / 2;
  float range = gesture_begin_top_freq - gesture_begin_bottom_freq;
  range /= scale;
  top_freq = center_freq + (range / 2);
  bottom_freq = center_freq - (range / 2);
  on_freq_change();
  queue_draw();
}

void Waterfall::on_gesture_pan(Gtk::PanDirection direction, double offset) {
  float hz = gesture_begin_top_freq - gesture_begin_bottom_freq;
  float hz_per_px = hz / get_allocation().get_width();
  float hz_pan = offset * hz_per_px;
  if (direction == Gtk::PanDirection::PAN_DIRECTION_RIGHT) {
    hz_pan = -hz_pan;
  }
  bottom_freq = gesture_begin_bottom_freq + hz_pan;
  top_freq = gesture_begin_top_freq + hz_pan;
  on_freq_change();
  queue_draw();
}

void Waterfall::on_gesture_swipe(double velocity_x, double velocity_y) {
  swipe_velocity = velocity_x * 0.05;
}

static void float_to_rgb(float f, unsigned char &r, unsigned char &g,
                         unsigned char &b) {
  float f_red = f * 3.0 - 2;
  float f_green = f * 3.0 - 1;
  float f_blue = f * 3 - 0;

  if (f_red < 0.0)
    f_red = 0.0;
  if (f_red > 1.0)
    f_red = 1.0;

  if (f_green < 0.0)
    f_green = 0.0;
  if (f_green > 1.0)
    f_green = 1.0;

  if (f_blue < 0.0)
    f_blue = 0.0;
  if (f_blue > 1.0)
    f_blue = 1.0;

  r = (unsigned char)(f_red * 256);
  g = (unsigned char)(f_green * 256);
  b = (unsigned char)(f_blue * 256);
}

// Called from GNU Radio thread
void Waterfall::add_fft(float *fft, unsigned size) {
  if (size != fft_size) {
    return;
  }

  memmove(background_data, background_data + background_stride,
          background_height * (background_stride - 1));
  unsigned char *new_row =
      background_data + background_stride * (background_height - 1);

  for (int i = fft_size / 2; i < fft_size; i++) {
    float value = (fft[i] - fft_min) / fft_scale;
    float_to_rgb(value, new_row[2], new_row[1], new_row[0]);
    new_row += 4;
  }
  for (int i = 0; i < fft_size / 2; i++) {
    float value = (fft[i] - fft_min) / fft_scale;
    float_to_rgb(value, new_row[2], new_row[1], new_row[0]);
    new_row += 4;
  }

  on_add_fft_dispatcher.emit();
}

void Waterfall::on_fft_added() { queue_draw(); }

void Waterfall::on_freq_change() { rig.setFreq((bottom_freq + top_freq) / 2); }
