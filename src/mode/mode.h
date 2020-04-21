#ifndef LPSDR_DEMOD_H
#define LPSDR_DEMOD_H

#include <glibmm/propertyproxy.h>
#include <gnuradio/hier_block2.h>
#include <gtkmm/widget.h>

class Mode : public Glib::Object {
public:
  typedef std::shared_ptr<Mode> sptr;

  virtual ~Mode(){};
  virtual gr::basic_block_sptr demod() = 0;

  /* Return a reference to a Widget allowing the mode to be configured.

  The widget will be added to the UI automatically, and removed before
  destroying the Mode. Thus, the Mode is responsible for deleting the Widget.

  This method should return the same instance throughout the Mode's lifetime.
  */
  virtual Gtk::Widget &settings_widget() = 0;

  virtual Glib::PropertyProxy<double> property_lower_cutoff() = 0;
  virtual Glib::PropertyProxy<double> property_upper_cutoff() = 0;
  virtual Glib::PropertyProxy<double> property_carrier_offset() = 0;

  double lower_cutoff() { return property_lower_cutoff().get_value(); };
  double upper_cutoff() { return property_upper_cutoff().get_value(); };
  double carrier_offset() { return property_carrier_offset().get_value(); };
};

#endif
