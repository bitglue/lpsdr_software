#ifndef LPSDR_DEMOD_H
#define LPSDR_DEMOD_H

#include <gnuradio/hier_block2.h>
#include <gtkmm/widget.h>

class Mode {
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
};

#endif
