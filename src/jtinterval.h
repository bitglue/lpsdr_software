#ifndef LPSDR_JTINTERVAL_H
#define LPSDR_JTINTERVAL_H

#include <chrono>

/* JTInterval implements time arithmetic for period-based modes, like WSPR,
 * JT9, etc. */

class JTInterval {
public:
  typedef std::chrono::system_clock::time_point time_point;
  typedef std::chrono::system_clock::duration duration;

  constexpr JTInterval(duration interval_seconds)
      : m_duration(interval_seconds){};

  /* Return the number of intervals that have elapsed since the epoch */
  constexpr unsigned number(const time_point t = now()) const {
    return t.time_since_epoch() / m_duration;
  };

  /* Return the time at which the interval started */
  constexpr time_point start(const time_point t = now()) const {
    return time_point() + number(t) * m_duration;
  };

  /* Return the duration elapsed for the given interval */
  constexpr duration since_start(const time_point t = now()) const {
    return t - start(t);
  };

  /* Return the time next interval will start */
  constexpr time_point next_start(const time_point t = now()) const {
    return start(t) + m_duration;
  };

  /* Return the time until the next interval starts */
  constexpr duration until_next(const time_point t = now()) const {
    return next_start(t) - t;
  };

  /* Return fraction of interval that has elapsed in [0, 1) */
  constexpr float fraction(const time_point t = now()) const {
    return float(since_start(t).count()) / float(m_duration.count());
  };

private:
  duration m_duration;
  static const time_point now() { return std::chrono::system_clock::now(); }
};

#endif
