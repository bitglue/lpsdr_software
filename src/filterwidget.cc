#include "filterwidget.h"
#include <cmath>

FilterWidget::FilterWidget()
    : Glib::ObjectBase(typeid(FilterWidget)),
      m_waterfall_center(*this, "waterfall-center"),
      m_waterfall_bandwidth(*this, "waterfall-bandwidth"),
      m_px_per_hz(*this, "px-per-hz"),
      m_lower_cutoff(*this, "lower-cutoff", 3000),
      m_upper_cutoff(*this, "upper-cutoff", -3000),
      m_carrier_offset(*this, "carrier-freq", -3000) {
  m_gesture_pan =
      Gtk::GesturePan::create(*this, Gtk::Orientation::ORIENTATION_HORIZONTAL);
  m_gesture_pan->set_propagation_phase(Gtk::PropagationPhase::PHASE_BUBBLE);
  m_gesture_pan->signal_pan().connect(
      sigc::mem_fun(*this, &FilterWidget::on_gesture_pan));

  property_waterfall_center().signal_changed().connect(
      sigc::mem_fun(*this, &FilterWidget::update_positions));
  property_px_per_hz().signal_changed().connect(
      sigc::mem_fun(*this, &FilterWidget::update_positions));
  property_carrier_offset().signal_changed().connect(
      sigc::mem_fun(*this, &FilterWidget::update_positions));
  property_lower_cutoff().signal_changed().connect(
      sigc::mem_fun(*this, &FilterWidget::update_positions));
  property_upper_cutoff().signal_changed().connect(
      sigc::mem_fun(*this, &FilterWidget::update_positions));
}

void FilterWidget::on_gesture_pan(Gtk::PanDirection direction, double offset) {
  if (direction == Gtk::PanDirection::PAN_DIRECTION_LEFT) {
    offset = -offset;
  }

  float px_per_hz = m_px_per_hz.get_value();
  auto offset_in_hz = offset / px_per_hz;
  float new_offset = carrier_offset() + offset_in_hz;
  m_carrier_offset = clamp_offset_to_valid_range(new_offset);
}

float FilterWidget::clamp_offset_to_valid_range(float offset) {
  float min = min_valid_carrier_offset(), max = max_valid_carrier_offset();
  if (offset < min)
    return min;
  if (offset > max)
    return max;
  return offset;
}

float FilterWidget::max_valid_carrier_offset() {
  float half_bw = m_waterfall_bandwidth.get_value() / 2;
  return half_bw - highest_edge();
}

float FilterWidget::min_valid_carrier_offset() {
  float half_bw = m_waterfall_bandwidth.get_value() / 2;
  return -half_bw - lowest_edge();
}

void FilterWidget::update_positions() {
  auto px_per_hz = m_px_per_hz.get_value();

  float width_in_hz = highest_edge() - lowest_edge();
  int width_req = ceil(width_in_hz * px_per_hz);
  if (width_req < 1)
    width_req = 1;
  set_size_request(width_req);

  float half_bw = m_waterfall_bandwidth.get_value() / 2;
  float margin_hz = carrier_offset() + lowest_edge() + half_bw;
  int margin_px = round(margin_hz * m_px_per_hz.get_value());
  set_margin_left(margin_px);
}

bool FilterWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  const int height = allocation.get_height();
  const auto px_per_hz = m_px_per_hz.get_value();

  cr->save();

  cr->scale(px_per_hz, 1);
  cr->set_line_width(1 / px_per_hz);
  cr->translate(-lowest_edge(), 0);

  cr->set_source_rgba(0.5, 0.5, 0.5, 0.2);
  cr->rectangle(lower_cutoff(), 0, upper_cutoff() - lower_cutoff(), height);
  cr->fill();

  cr->set_source_rgba(1, 0, 0, 0.7);
  cr->move_to(0, 0);
  cr->line_to(0, height);
  cr->stroke();

  cr->restore();
  return true;
}

float FilterWidget::lowest_edge() const {
  float lowest_edge = 0;
  float lower = m_lower_cutoff.get_value(), upper = m_upper_cutoff.get_value();
  if (lower < lowest_edge) {
    lowest_edge = lower;
  }
  if (upper < lowest_edge) {
    lowest_edge = upper;
  }
  return lowest_edge;
}

float FilterWidget::highest_edge() const {
  float highest_edge = 0;
  float lower = m_lower_cutoff.get_value(), upper = m_upper_cutoff.get_value();
  if (lower > highest_edge) {
    highest_edge = lower;
  }
  if (upper > highest_edge) {
    highest_edge = upper;
  }
  return highest_edge;
}
