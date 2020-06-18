#include "catch.hpp"
#include "si5351.h"

class MockRegisters : public Si5351RegisterInterface {
public:
  void write(unsigned reg, uint8_t value) { m_registers[reg] = value; };

  uint8_t read(unsigned reg) const {
    assert(reg <= 187);
    return m_registers[reg];
  };

  void get_pll_config(uint32_t &P1, uint32_t &P2, uint32_t &P3, bool pll_b) {
    unsigned reg = pll_b ? SI_SYNTH_PLL_B : SI_SYNTH_PLL_A;

    P1 = P2 = P3 = 0;

    P3 |= read(reg + 0) << 8;
    P3 |= read(reg + 1);
    P1 |= (read(reg + 2) & 0x3) << 16;
    P1 |= read(reg + 3) << 8;
    P1 |= read(reg + 4);
    P3 |= (read(reg + 5) & 0xf0) << 12;
    P2 |= (read(reg + 5) & 0x0f) << 16;
    P2 |= read(reg + 6) << 8;
    P2 |= read(reg + 7);
  };

  uint8_t m_registers[188];
};

TEST_CASE("Si5351", "[si5351]") {
  auto registers = MockRegisters();
  auto si5351 = Si5351(registers);

  SECTION("reset PLL A") {
    si5351.resetPLL(true, false);
    CHECK(registers.read(177) == 0x20);
  }

  SECTION("reset PLL B") {
    si5351.resetPLL(false, true);
    CHECK(registers.read(177) == 0x80);
  }

  SECTION("reset PLL A and B") {
    si5351.resetPLL(true, true);
    CHECK(registers.read(177) == 0xA0);
  }

  //  SECTION("setup PLL A") {
  //    si5351.setupPLL(0, 33 * 1048575 + 349524, 1048575);
  //
  //    unsigned P1, P2, P3;
  //    registers.get_pll_config(P1, P2, P3, false);
  //
  //    CHECK(P1 == 3754);
  //    CHECK(P2 == 698922);
  //    CHECK(P3 == 1048575);
  //  }

  SECTION("disable outputs") {
    si5351.disableOutputs();
    CHECK(registers.read(3) == 0xff);
  }

  SECTION("disable output") {
    registers.write(3, 0);
    si5351.disableOutput(Si5351Output::OUT_0);
    CHECK(registers.read(3) == 0x01);
    si5351.disableOutput(Si5351Output::OUT_1);
    CHECK(registers.read(3) == 0x03);
    si5351.disableOutput(Si5351Output::OUT_2);
    CHECK(registers.read(3) == 0x07);
    si5351.disableOutput(Si5351Output::OUT_3);
    CHECK(registers.read(3) == 0x0f);
    // si5351.disableOutput(Si5351Output::OUT_7);
    // CHECK(registers.read(3) == 0x8f);
  }

  SECTION("enable output") {
    registers.write(3, 0xff);
    si5351.enableOutput(Si5351Output::OUT_0);
    CHECK(registers.read(3) == 0xfe);
    si5351.enableOutput(Si5351Output::OUT_1);
    CHECK(registers.read(3) == 0xfc);
    si5351.enableOutput(Si5351Output::OUT_2);
    CHECK(registers.read(3) == 0xf8);
    si5351.enableOutput(Si5351Output::OUT_3);
    CHECK(registers.read(3) == 0xf0);
    // si5351.enableOutput(Si5351Output::OUT_7);
    // CHECK(registers.read(3) == 0x70);
  }

  SECTION("set phase output") {
    si5351.setPhaseOffset(Si5351Output::OUT_0, 13);
    CHECK(registers.read(165) == 13);

    si5351.setPhaseOffset(Si5351Output::OUT_1, 42);
    CHECK(registers.read(166) == 42);

    si5351.setPhaseOffset(Si5351Output::OUT_5, 127);
    CHECK(registers.read(170) == 127);

    // offset out of range
    CHECK_THROWS(si5351.setPhaseOffset(Si5351Output::OUT_0, 128));
  }

  SECTION("set clk control") {
    Si5351ClkCtrl ctrl;

    CHECK(ctrl.power_down == false);
    CHECK(ctrl.int_mode == false);
    CHECK(ctrl.ms_source == Si5351PLL::PLL_A);
    CHECK(ctrl.invert == false);
    CHECK(ctrl.drive_strength == Si5351DrvStr::mA_2);

    si5351.setClkCtrl(Si5351Output::OUT_0, ctrl);
    CHECK((int)registers.read(16) == 0x0c);

    ctrl.invert = true;
    si5351.setClkCtrl(Si5351Output::OUT_1, ctrl);
    CHECK((int)registers.read(17) == 0x1c);
  }
}

TEST_CASE("Si5351Rational", "[si5351]") {
  SECTION(".multiplier()") {
    Si5351Rational mult(3417, 6, 10);
    CHECK(mult.multiplier() == Rational(307, 10));
  }

  SECTION("construct from double") {
    Si5351Rational mult(30.7);
    CHECK(mult.multiplier() == Rational(307, 10));

    double f = 33.13471261728395;
    auto m = Si5351Rational(f);
    CHECK(double(m.multiplier()) == Approx(f));
  }

  SECTION("construct from rational") {
    Rational r(307, 10);
    Si5351Rational mult(r);
    CHECK(mult.multiplier() == r);
  }

  SECTION("construct from invalid rational") {
    // Denominator is larger than 0xfffff, and so requires approximation.
    Rational r(1341955861, 40500000);
    CHECK_THROWS(Si5351Rational(r));
  }

  SECTION("construct by approximating an invalid rational") {
    // Denominator is larger than 0xfffff, and so requires approximation.
    Rational r(1341955861, 40500000);
    auto m = Si5351Rational::approximate(r);
    CHECK(m.multiplier() == Rational(23664381, 714187));
  }

  SECTION("construct from double that happens to be an integer") {
    Si5351Rational mult(40.0);
    CHECK(mult.multiplier() == Rational(40, 1));
    CHECK(mult.p3() == 1);
  }

  SECTION("construct from unsigned") {
    Si5351Rational mult(30U);
    CHECK(mult.multiplier() == Rational(30, 1));
    CHECK(mult.p3() == 1);
  }
}

TEST_CASE("Si5351FreqPlan", "[si5351]") {
  Rational xtal_freq(27000000, 1);

  SECTION("10 MHz") {
    // It's possible to get exactly 10 MHz:
    // 27 MHz * 100 / 3 = 900 MHz
    // 900 MHz / 90 = 10 MHz

    Rational const out_freq(10000000, 1);
    Si5351FreqPlan plan(xtal_freq, out_freq);
    CHECK(plan.is_invalid() == NULL);
    CHECK(plan.output_freq() == out_freq);
  }

  SECTION("7 MHz") {
    // 7 MHz is possible to get exactly:
    //
    // 27 MHz * 100 * 7 / 27 = 700 MHz
    // 700 MHz / 100 = 7 MHz
    //
    // However, this requires using something other than 0xfffff as the PLL
    // denominator. (27 = 3*3*3, and 0xfffff has only one 3 in its prime
    // factorization)
    Rational const out_freq(7000000, 1);
    Si5351FreqPlan plan(xtal_freq, out_freq);
    CHECK(plan.is_invalid() == NULL);
    CHECK(plan.output_freq() == out_freq);
  }

  SECTION("7000001 Hz") {
    // 27 MHz * 33 * 78127/421875 = 896.000128 MHz
    // 896.000128 / 128 = 7.000001 MHz
    Rational const out_freq(7000001, 1);
    Si5351FreqPlan plan(xtal_freq, out_freq);
    CHECK(plan.is_invalid() == NULL);
    CHECK(plan.output_freq() == out_freq);
  }

  SECTION("about 6.482878555555556 MHz") {
    // The exact solution is:
    //
    // 27 * 1341955861 / 40500000 / 138 = 58345907 / 9
    //
    // But the 40500000 denominator is too large for the maximum 1048575.
    // Consequently, we must accept an approximation.

    Rational const out_freq(58345907, 9);
    Si5351FreqPlan plan(xtal_freq, out_freq);
    CHECK(plan.is_invalid() == NULL);
    CHECK(double(plan.output_freq()) == Approx(double(out_freq)));
  }

  /*
  SECTION("113 MHz") {
    // Valid output multisynth divisors
    double const out_freq = 113e6;
    si5351.calculateParams(xtal_freq, out_freq, params);
    CHECK(params.output_freq(xtal_freq) == out_freq);
  }
  */
}

TEST_CASE("Rational", "[si5351]") {
  SECTION("multiply rational") {
    Rational a(2, 3);
    Rational b(1, 3);
    CHECK(a * b == Rational(2, 9));
  }

  SECTION("multiply integer") { CHECK(Rational(1, 3) * 2 == Rational(2, 3)); }

  SECTION("divide rational") {
    Rational a(2, 3);
    Rational b(2, 1);
    CHECK(a / b == Rational(1, 3));
  }

  SECTION("divide integer") { CHECK(Rational(1, 3) / 2 == Rational(1, 6)); }

  SECTION("equality and reduction") {
    CHECK(Rational(5, 10) == Rational(1, 2));
  }

  SECTION("inequality") {
    CHECK(Rational(5, 11) != Rational(6, 11));
    CHECK(Rational(5, 11) != Rational(5, 12));
    CHECK(!(Rational(5, 11) != Rational(5, 11)));
  }

  SECTION("approximate a double") {
    Rational a = Rational::approximate(1.0 / 3.0, 1000);
    CHECK(a == Rational(1, 3));

    Rational b = Rational::approximate(0.36787944117144233, 10);
    CHECK(b == Rational(3, 8));

    Rational c = Rational::approximate(0.36787944117144233, 100);
    CHECK(c == Rational(32, 87));

    Rational d = Rational::approximate(20.36787944117144233, 100);
    CHECK(d == Rational(20 * 87 + 32, 87));
  }

  SECTION("cast to a double") {
    CHECK(double(Rational(4, 1)) == 4.0);
    CHECK(double(Rational(58345907, 9)) == 6482878.555555556);
  }

  SECTION("cast to uint64_t") {
    CHECK((uint64_t)Rational(4, 1) == 4);
    CHECK((uint64_t)Rational(40, 10) == 4);
    CHECK((uint64_t)Rational(49, 10) == 4);
  }
}
