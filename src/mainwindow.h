#ifndef LPSDR_MAINWINDOW_H
#define LPSDR_MAINWINDOW_H

#include "flowgraph.h"
#include "waterfall.h"
#include <glibmm/dispatcher.h>
#include <gtkmm/window.h>

class MainWindow : public Gtk::Window {
public:
  MainWindow();
  virtual ~MainWindow();

protected:
  Waterfall waterfall;
  Flowgraph flowgraph;
  virtual bool on_delete_event(GdkEventAny *any_event) override;
};

#endif
