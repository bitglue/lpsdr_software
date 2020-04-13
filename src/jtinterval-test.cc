#include "catch.hpp"
#include "jtinterval.h"
#include <ctime>
#include <iostream>

using namespace std::chrono;

// Return a time_point representing the given UTC time on Apr 22, 2020
system_clock::time_point hms(int hours, int minutes, int seconds) {
  struct tm tm_now;
  tm_now.tm_year = 120;
  tm_now.tm_mon = 3;
  tm_now.tm_mday = 22;
  tm_now.tm_hour = hours;
  tm_now.tm_min = minutes;
  tm_now.tm_sec = seconds;
  time_t tt_now = timegm(&tm_now);

  return system_clock::from_time_t(tt_now);
}

TEST_CASE("time intervals", "[jtinterval]") {
  JTInterval interval(seconds(120));

  SECTION("number") {
    REQUIRE(interval.number(hms(0, 0, 0)) == 13229280);
    REQUIRE(interval.number(hms(0, 0, 30)) == 13229280);
    REQUIRE(interval.number(hms(0, 1, 59)) == 13229280);
    REQUIRE(interval.number(hms(0, 2, 0)) == 13229281);
    REQUIRE(interval.number(hms(0, 3, 0)) == 13229281);
  }

  SECTION("start") {
    REQUIRE(interval.start(hms(0, 0, 0)) == hms(0, 0, 0));
    REQUIRE(interval.start(hms(0, 0, 30)) == hms(0, 0, 0));
    REQUIRE(interval.start(hms(0, 1, 59)) == hms(0, 0, 0));
    REQUIRE(interval.start(hms(0, 2, 0)) == hms(0, 2, 0));
    REQUIRE(interval.start(hms(0, 3, 0)) == hms(0, 2, 0));
  }

  SECTION("since_start") {
    REQUIRE(interval.since_start(hms(0, 0, 0)) == seconds(0));
    REQUIRE(interval.since_start(hms(0, 0, 30)) == seconds(30));
    REQUIRE(interval.since_start(hms(0, 1, 59)) == seconds(119));
    REQUIRE(interval.since_start(hms(0, 2, 0)) == seconds(0));
    REQUIRE(interval.since_start(hms(0, 3, 0)) == seconds(60));
  }

  SECTION("next_start") {
    REQUIRE(interval.next_start(hms(0, 0, 0)) == hms(0, 2, 0));
    REQUIRE(interval.next_start(hms(0, 0, 30)) == hms(0, 2, 0));
    REQUIRE(interval.next_start(hms(0, 1, 59)) == hms(0, 2, 0));
    REQUIRE(interval.next_start(hms(0, 2, 0)) == hms(0, 4, 0));
    REQUIRE(interval.next_start(hms(0, 3, 0)) == hms(0, 4, 0));
  }

  SECTION("until_next") {
    REQUIRE(interval.until_next(hms(0, 0, 0)) == seconds(120));
    REQUIRE(interval.until_next(hms(0, 0, 30)) == seconds(90));
    REQUIRE(interval.until_next(hms(0, 1, 59)) == seconds(1));
    REQUIRE(interval.until_next(hms(0, 2, 0)) == seconds(120));
    REQUIRE(interval.until_next(hms(0, 3, 0)) == seconds(60));
  }

  SECTION("fraction") {
    REQUIRE(interval.fraction(hms(0, 0, 0)) == 0.0);
    REQUIRE(interval.fraction(hms(0, 0, 30)) == 0.25);
    REQUIRE(std::abs(interval.fraction(hms(0, 1, 59)) - 0.99167) < 0.00001);
    REQUIRE(interval.fraction(hms(0, 2, 0)) == 0.0);
    REQUIRE(interval.fraction(hms(0, 3, 0)) == 0.5);
  }
}
