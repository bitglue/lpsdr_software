#ifndef LPSDR_SI5351_H
#define LPSDR_SI5351_H

#include <inttypes.h>
#include <stdexcept>

// Register definitions
#define SI_OUTPUT_ENABLE 3
#define SI_CLK0_CONTROL 16
#define SI_CLK1_CONTROL 17
#define SI_CLK2_CONTROL 18
#define SI_CLK3_CONTROL 19
#define SI_CLK4_CONTROL 20
#define SI_CLK5_CONTROL 21
#define SI_CLK6_CONTROL 22
#define SI_CLK7_CONTROL 23
#define SI_SYNTH_PLL_A 26
#define SI_SYNTH_PLL_B 34
#define SI_SYNTH_MS_0 42
#define SI_SYNTH_MS_1 50
#define SI_SYNTH_MS_2 58
#define SI_SYNTH_MS_3 66
#define SI_SYNTH_MS_4 74
#define SI_SYNTH_MS_5 82
#define SI_CLK0_PHOFF 165
#define SI_CLK1_PHOFF 166
#define SI_CLK2_PHOFF 167
#define SI_CLK3_PHOFF 168
#define SI_CLK4_PHOFF 169
#define SI_CLK5_PHOFF 170
#define SI_PLL_RESET 177

#define SI_CLK_SRC_PLL_A 0x00
#define SI_CLK_SRC_PLL_B 0x20

#define XTAL_FREQ 27000000 // Crystal frequency

void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency(uint32_t frequency);

enum class Si5351RDiv : uint8_t {
  R_DIV_1 = 0,
  R_DIV_2 = 1,
  R_DIV_4 = 2,
  R_DIV_8 = 3,
  R_DIV_16 = 4,
  R_DIV_32 = 5,
  R_DIV_64 = 6,
  R_DIV_128 = 7,
};

enum class Si5351Output : uint8_t {
  OUT_0 = 0,
  OUT_1 = 1,
  OUT_2 = 2,
  OUT_3 = 3,
  OUT_4 = 4,
  OUT_5 = 5,
  OUT_6 = 6,
};

enum class Si5351PLL : uint8_t {
  PLL_A = 0,
  PLL_B = 1,
};

class Rational {
public:
  constexpr Rational(uint64_t numerator, uint32_t denominator)
      : m_num(numerator / gcd(numerator, denominator)),
        m_denom(denominator / gcd(numerator, denominator)){};

  // Returns the best approximation with a denominator =< max_denom
  static Rational approximate(double f, uint32_t max_denom);

  constexpr Rational operator*(const Rational &other) {
    return Rational(m_num * other.m_num, m_denom * other.m_denom);
  };

  constexpr Rational operator*(uint64_t other) {
    return Rational(m_num * other, m_denom);
  };

  constexpr Rational operator/(const Rational &other) {
    return Rational(m_num * other.m_denom, m_denom * other.m_num);
  };

  constexpr Rational operator/(uint64_t other) {
    return Rational(m_num, m_denom * other);
  };

  constexpr bool operator==(const Rational &other) {
    return m_num == other.m_num && m_denom == other.m_denom;
  }

  constexpr bool operator!=(const Rational &other) { return !(*this == other); }

  constexpr explicit operator double() { return double(m_num) / m_denom; };
  constexpr explicit operator uint64_t() { return m_num / m_denom; };

  constexpr uint64_t numerator() { return m_num; };
  constexpr uint64_t denominator() { return m_denom; };

private:
  uint64_t m_num;
  uint32_t m_denom;

  static constexpr uint64_t gcd(uint64_t a, uint64_t b) {
    return b ? gcd(b, a % b) : a;
  };
};

class Si5351Rational {
public:
  static Si5351Rational approximate(Rational m);

  Si5351Rational(uint32_t ip1, uint32_t ip2, uint32_t ip3);
  Si5351Rational(unsigned m) : Si5351Rational(Rational(m, 1)){};
  Si5351Rational(double m)
      : Si5351Rational(Rational::approximate(m, 0xfffff)){};
  Si5351Rational(Rational m)
      : Si5351Rational(uint64_t(m * 128) - 512,
                       (m * 128).numerator() % (m * 128).denominator(),
                       (m * 128).denominator()){};

  Rational multiplier() const {
    return Rational((m_p1 + 512) * m_p3 + m_p2, m_p3 * 128);
  };

  uint32_t p1() const { return m_p1; };
  uint32_t p2() const { return m_p2; };
  uint32_t p3() const { return m_p3; };

private:
  uint32_t m_p1, m_p2, m_p3;
};

class Si5351FreqPlan {
public:
  Si5351FreqPlan(Rational xtal_freq, Rational out_freq);
  const char *is_invalid();

  Rational pll_freq() { return m_xtal_freq * m_pll_mult.multiplier(); };

  Rational output_freq() {
    return pll_freq() / m_output_div.multiplier() / (1 << (int)m_output_r_div);
  };

  const Si5351Rational &pll_mult() { return m_pll_mult; };
  const Si5351Rational &output_div() { return m_output_div; };
  Si5351RDiv output_r_div() { return m_output_r_div; };

private:
  Rational m_xtal_freq;
  Si5351Rational m_pll_mult;
  Si5351Rational m_output_div;
  Si5351RDiv m_output_r_div;
};

class Si5351RegisterInterface {
public:
  virtual ~Si5351RegisterInterface(){};
  virtual void write(unsigned reg, uint8_t value) = 0;
  virtual uint8_t read(unsigned reg) const = 0;
};

class Si5351 {
public:
  Si5351(Si5351RegisterInterface &);
  virtual ~Si5351();

  void setupPLL(Si5351PLL pll, const Si5351Rational &);
  void setupOutputDivider(Si5351Output out, const Si5351Rational &divider,
                          Si5351RDiv rDiv);
  void setPhaseOffset(Si5351Output out, unsigned offset);
  void resetPLL(bool reset_a, bool reset_b);
  void disableOutputs();
  void disableOutput(Si5351Output out);
  void enableOutput(Si5351Output out);
  void setClkControl(unsigned clk, bool power_down, bool int_mode, bool invert,
                     uint8_t drive_strength);

  static constexpr double MIN_PLL_FREQ = 600e6;
  static constexpr double MAX_PLL_FREQ = 900e6;

private:
  Si5351RegisterInterface &m_registers;
};

#endif // LPSDR_SI5351_H
