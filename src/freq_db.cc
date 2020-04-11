#include "freq_db.h"
#include <cmath>
#include <limits>

// must be non-overlapping and ordered from lowest to highest frequency
const std::array<const Band, 11> FrequencyDB::m_bands = {
    Band("2,200m", 135.7e3, 137.8e3), Band("630m", 472e3, 479e3),
    Band("160m", 1.8e6, 2e6),         Band("80m", 3.5e6, 4e6),
    Band("40m", 7e6, 7.3e6),          Band("30m", 10.1e6, 10.150e6),
    Band("20m", 14e6, 14.350e6),      Band("17m", 18.068e6, 18.168e6),
    Band("15m", 21e6, 21.450e6),      Band("12m", 24.89e6, 24.99e6),
    Band("10m", 28e6, 29.7e6),
};

const Band &FrequencyDB::band_below(float f) const {
  auto i = closest_band_index(f);
  if (i >= 1) {
    i -= 1;
  }
  return m_bands.at(i);
}

const Band &FrequencyDB::band_above(float f) const {
  auto i = closest_band_index(f);
  if (i < m_bands.size() - 1) {
    i += 1;
  }
  return m_bands.at(i);
}

unsigned FrequencyDB::closest_band_index(float f) const {
  float best_distance = std::numeric_limits<float>::infinity();
  unsigned best_index = 0;

  for (unsigned i = 0; i < m_bands.size(); i++) {
    float distance = std::abs(m_bands.at(i).distance_to(f));
    if (distance < best_distance) {
      best_distance = distance;
      best_index = i;
    }
  }

  assert(best_index < m_bands.size());
  return best_index;
}
