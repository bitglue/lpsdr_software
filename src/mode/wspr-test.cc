#include "catch.hpp"
#include "mode/wspr.h"
#include <gnuradio/analog/noise_source_c.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/top_block.h>
#include <gtkmm/box.h>

const unsigned sample_rate = 48000;

TEST_CASE("WSPR demodulation", "[wspr]") {
  auto wspr = WSPR::make(sample_rate);
  REQUIRE(wspr != nullptr);

  auto demod = wspr->demod();
  REQUIRE(demod != nullptr);

  SECTION("settings widget can be added and removed from a container") {
    Gtk::Box box;

    REQUIRE(box.get_children().size() == 0);
    box.add(wspr->settings_widget());
    REQUIRE(box.get_children().size() == 1);
    box.remove(wspr->settings_widget());
    REQUIRE(box.get_children().size() == 0);
  }

  SECTION("demod block can run") {
    auto top_block = gr::make_top_block("wspr test");
    auto noise_source = gr::analog::noise_source_c::make(
        gr::analog::noise_type_t::GR_UNIFORM, 0.1);
    auto head = gr::blocks::head::make(sizeof(gr_complex), sample_rate * 240);

    top_block->connect(noise_source, 0, head, 0);
    top_block->connect(head, 0, demod, 0);
    top_block->start();
    top_block->wait();
  }
}
