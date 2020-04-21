#ifndef LPSDR_FILTERWIDGET_H
#define LPSDR_FILTERWIDGET_H

#include <glibmm/property.h>
#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturepan.h>

class FilterWidget : public Gtk::DrawingArea {
public:
  FilterWidget();

  Glib::PropertyProxy<float> property_waterfall_center() {
    return m_waterfall_center.get_proxy();
  }
  Glib::PropertyProxy<float> property_waterfall_bandwidth() {
    return m_waterfall_bandwidth.get_proxy();
  }
  Glib::PropertyProxy<float> property_px_per_hz() {
    return m_px_per_hz.get_proxy();
  }
  Glib::PropertyProxy<float> property_lower_cutoff() {
    return m_lower_cutoff.get_proxy();
  }
  Glib::PropertyProxy<float> property_upper_cutoff() {
    return m_upper_cutoff.get_proxy();
  }
  Glib::PropertyProxy<float> property_carrier_offset() {
    return m_carrier_offset.get_proxy();
  }

  float lower_cutoff() { return m_lower_cutoff.get_value(); }
  float upper_cutoff() { return m_upper_cutoff.get_value(); }
  float carrier_offset() { return m_carrier_offset.get_value(); }

  float min_valid_carrier_offset();
  float max_valid_carrier_offset();
  float clamp_offset_to_valid_range(float offset);

protected:
  Glib::RefPtr<Gtk::GesturePan> m_gesture_pan;

  Glib::Property<float> m_waterfall_center;
  Glib::Property<float> m_waterfall_bandwidth;
  Glib::Property<float> m_px_per_hz;
  Glib::Property<float> m_lower_cutoff;
  Glib::Property<float> m_upper_cutoff;
  Glib::Property<float> m_carrier_offset;

  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
  void on_gesture_pan(Gtk::PanDirection direction, double offset);
  void update_positions();
  float lowest_edge() const;
  float highest_edge() const;
};

#endif
