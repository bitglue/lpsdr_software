#ifndef LPSDR_FREQ_DB_H
#define LPSDR_FREQ_DB_H

#include <array>
#include <cassert>

struct Band {
  const char *const name;
  const float lower, upper;

  constexpr Band(const char *const n, float l, float u)
      : name(n), lower(l), upper(u){};

  /* Return true iff `f` is within the band. */
  bool contains(float f) const { return f >= lower && f <= upper; };

  /* Return the distance from the closest band edge to `f`.

     If `f` is below the band, the returned value will be negative.

     If `f` is above the band, positive.

     If `f` is within the band, returns 0. */
  float distance_to(float f) const {
    if (f < lower) {
      return f - lower;
    }
    if (f > upper) {
      return f - upper;
    }
    assert(contains(f));
    return 0;
  };

  /* Return the frequency at the center of the band. */
  float center() const { return (lower + upper) / 2; };
};

class FrequencyDB {
public:
  const Band &band_below(float) const;
  const Band &band_above(float) const;

protected:
  static const std::array<const Band, 11> m_bands;
  unsigned closest_band_index(float) const;
};

#endif
