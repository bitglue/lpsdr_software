#include "si5351.h"
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>

class PigpioRegisters : public Si5351RegisterInterface {
public:
  PigpioRegisters(int i2c) : m_i2c(i2c){};
  void write(unsigned reg, uint8_t value) {
    printf("%i <- 0x%02x\n", reg, value);
    i2cWriteByteData(m_i2c, reg, value);
  };
  uint8_t read(unsigned reg) const { return i2cReadByteData(m_i2c, reg); };

protected:
  int m_i2c;
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    puts("one argument required");
    return 1;
  }

  unsigned freq = atoi(argv[1]);
  gpioInitialise();
  int i2c = i2cOpen(1, 0x60, 0);
  {
    auto registers = PigpioRegisters(i2c);
    auto si5351 = Si5351(registers);

    Si5351FreqPlan plan(Rational(XTAL_FREQ, 1), Rational(freq, 1));
    si5351.setupPLL(Si5351PLL::PLL_A, plan.pll_mult());
    si5351.setupOutputDivider(Si5351Output::OUT_4, plan.output_div(),
                              plan.output_r_div());
    si5351.setupOutputDivider(Si5351Output::OUT_5, plan.output_div(),
                              plan.output_r_div());
    si5351.setPhaseOffset(Si5351Output::OUT_5,
                          uint64_t(plan.output_div().multiplier()));
    si5351.resetPLL(true, true);
    registers.write(SI_CLK4_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
    registers.write(SI_CLK5_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
  }
  i2cClose(i2c);
  gpioTerminate();
}
