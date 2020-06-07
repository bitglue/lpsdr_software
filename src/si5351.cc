#include "si5351.h"
#include <cassert>
#include <cmath>

#define I2C_WRITE 0x60
#define I2C_READ 0x61
#define I2C_BUS 1

Si5351::Si5351(Si5351RegisterInterface &registerInterface)
    : m_registers(registerInterface) {}

Si5351::~Si5351() {}

static void farey(double x, unsigned N, unsigned &numerator,
                  unsigned &denominator) {
  // find the best rational approximation of x (between 0 and 1, inclusive) with
  // a denominator not larger than N
  unsigned a = 0, b = 1;
  unsigned c = 1, d = 1;
  double mediant;
  while (b <= N and d <= N) {
    mediant = double(a + c) / (b + d);
    if (x == mediant) {
      if (b + d <= N) {
        numerator = a + c;
        denominator = b + d;
        return;
      }
      if (d > b) {
        numerator = c;
        denominator = d;
        return;
      }
      numerator = a;
      denominator = b;
      return;
    }

    if (x > mediant) {
      a += c;
      b += d;
    } else {
      c += a;
      d += b;
    }
  }

  if (b > N) {
    numerator = c;
    denominator = d;
    return;
  }

  numerator = a;
  denominator = b;
  return;
}

Rational Rational::approximate(double f, uint32_t max_denom) {
  double int_part, frac_part;
  unsigned num, denom;
  frac_part = modf(f, &int_part);
  farey(frac_part, max_denom, num, denom);
  return Rational(denom * int_part + num, denom);
}

Si5351Rational::Si5351Rational(uint32_t p1, uint32_t p2, uint32_t p3)
    : m_p1(p1), m_p2(p2), m_p3(p3) {
  if (p1 > 0x7ffff)
    throw std::runtime_error("P1 does not fit in 17 bits");
  if (p2 > 0xfffff)
    throw std::runtime_error("P2 does not fit in 20 bits");
  if (p3 > 0xfffff)
    throw std::runtime_error("P3 does not fit in 30 bits");
}

Si5351Rational Si5351Rational::approximate(Rational m) {
  if (m.denominator() > 0xfffff) {
    return Si5351Rational(double(m));
  }

  return Si5351Rational(m);
}

Si5351FreqPlan::Si5351FreqPlan(Rational xtal_freq, Rational out_freq)
    : m_xtal_freq(xtal_freq), m_pll_mult(0, 0, 0), m_output_div(0, 0, 0),
      m_output_r_div(Si5351RDiv::R_DIV_1) {
  // We must find PLL frequency which is:
  //
  // - between MIN_PLL_FREQ and MAX_PLL_FREQ, to respect device specs, and
  // - an integer multiple of the output frequency, for best jitter
  // performance.
  //
  // So we begin by calculating the valid bounds for that integer multiple.
  // unsigned min_output_div = ceil(MIN_PLL_FREQ / out_freq);
  unsigned max_output_div = floor(Si5351::MAX_PLL_FREQ / double(out_freq));

  // Using the maximum multiple means the smallest steps between increments
  // of the PLL numerator. So absent other constraints, we'll pick that.
  //
  // Now we can calculate the PLL multiplier as a double. We've already
  // decided the output divider will be an integer.
  Rational pll_mult = out_freq / xtal_freq * max_output_div;
  m_output_div = Si5351Rational(max_output_div);
  m_pll_mult = Si5351Rational::approximate(pll_mult);

  assert(is_invalid() == NULL);
}

const char *Si5351FreqPlan::is_invalid() {
  double f = double(pll_freq());
  if (f < 600e6)
    return "PLL frequency is below 600 MHz";
  if (f > 900e6)
    return "PLL frequency is above 900 MHz";

  auto output_div = m_output_div.multiplier();

  // 3.1.3: output divider must be 4 when 150 MHz < Fout <= 200 MHz
  if (output_div != Rational(4, 1)) {
    // 3.1.2: output divider must be "between 8 + 1/1,048,575 and 2048."
    if (double(output_div) < 8 + 1.0 / 0xffff)
      return "Output divider is less than 8+1/0xfffff";
    if (double(output_div) > 2048)
      return "Output divider is greater than 2048";
  }

  return NULL;
}

void Si5351::setupPLL(Si5351PLL pll, const Si5351Rational &mult) {
  uint32_t P1 = mult.p1();
  uint32_t P2 = mult.p2();
  uint32_t P3 = mult.p3();

  unsigned reg = pll == Si5351PLL::PLL_B ? SI_SYNTH_PLL_B : SI_SYNTH_PLL_A;

  m_registers.write(reg + 0, (P3 & 0x0000FF00) >> 8);
  m_registers.write(reg + 1, (P3 & 0x000000FF));
  m_registers.write(reg + 2, (P1 & 0x00030000) >> 16);
  m_registers.write(reg + 3, (P1 & 0x0000FF00) >> 8);
  m_registers.write(reg + 4, (P1 & 0x000000FF));
  m_registers.write(reg + 5,
                    ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  m_registers.write(reg + 6, (P2 & 0x0000FF00) >> 8);
  m_registers.write(reg + 7, (P2 & 0x000000FF));
}

void Si5351::setupOutputDivider(Si5351Output out, const Si5351Rational &divider,
                                Si5351RDiv rDiv) {
  uint32_t P1 = divider.p1();
  uint32_t P2 = divider.p2();
  uint32_t P3 = divider.p3();

  unsigned base_register = SI_SYNTH_MS_0 + int(out) * 8;

  m_registers.write(base_register + 0, (P3 & 0x0000FF00) >> 8);
  m_registers.write(base_register + 1, (P3 & 0x000000FF));
  m_registers.write(base_register + 2, ((P1 & 0x00030000) >> 16) | int(rDiv));
  m_registers.write(base_register + 3, (P1 & 0x0000FF00) >> 8);
  m_registers.write(base_register + 4, (P1 & 0x000000FF));
  m_registers.write(base_register + 5,
                    ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  m_registers.write(base_register + 6, (P2 & 0x0000FF00) >> 8);
  m_registers.write(base_register + 7, (P2 & 0x000000FF));
}

void Si5351::setPhaseOffset(Si5351Output out, unsigned offset) {
  if (offset > 0x7f) {
    throw std::runtime_error("Phase offset is greater than 0x7f");
  }
  m_registers.write(SI_CLK0_PHOFF + int(out), offset);
}

void Si5351::resetPLL(bool reset_a, bool reset_b) {
  unsigned reset = 0;
  if (reset_a)
    reset |= 0x20;
  if (reset_b)
    reset |= 0x80;
  m_registers.write(SI_PLL_RESET, reset);
}

void Si5351::disableOutputs() { m_registers.write(SI_OUTPUT_ENABLE, 0xff); }

void Si5351::disableOutput(Si5351Output out) {
  auto enabled = m_registers.read(SI_OUTPUT_ENABLE);
  enabled |= 1 << int(out);
  m_registers.write(SI_OUTPUT_ENABLE, enabled);
}

void Si5351::enableOutput(Si5351Output out) {
  auto enabled = m_registers.read(SI_OUTPUT_ENABLE);
  enabled &= ~(1 << int(out));
  m_registers.write(SI_OUTPUT_ENABLE, enabled);
}

void Si5351::setClkControl(unsigned clk, bool power_down, bool int_mode,
                           bool invert, uint8_t drive_strength) {
  m_registers.write(16 + clk, (power_down << 7) | (int_mode << 6) |
                                  (invert << 4) | (drive_strength & 3));
}
