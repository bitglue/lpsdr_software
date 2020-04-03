#ifndef LPSDR_RIG_BREADBOARD_H
#define LPSDR_RIG_BREADBOARD_H

#include "rig.h"

class Breadboard : public LPSDRRig {
public:
  Breadboard();
  ~Breadboard();
  gr::basic_block_sptr source();
  void set_freq(double);
  double get_freq();

protected:
  int pigpio;
  double last_freq;
  void setupPLL(int i2c, uint8_t pll, uint8_t mult, uint32_t num,
                uint32_t denom);
  void setupMultisynth(int i2c, uint8_t synth, uint32_t divider, uint8_t rDiv);
  void si5351aOutputOff(uint8_t clk);
  void si5351aSetFrequency(uint32_t frequency);
};

#define SI_CLK0_CONTROL 16 // Register definitions
#define SI_CLK1_CONTROL 17
#define SI_CLK2_CONTROL 18
#define SI_SYNTH_PLL_A 26
#define SI_SYNTH_PLL_B 34
#define SI_SYNTH_MS_0 42
#define SI_SYNTH_MS_1 50
#define SI_SYNTH_MS_2 58
#define SI_PLL_RESET 177

#define SI_R_DIV_1 0x00 // R-division ratio definitions
#define SI_R_DIV_2 0x10
#define SI_R_DIV_4 0x20
#define SI_R_DIV_8 0x30
#define SI_R_DIV_16 0x40
#define SI_R_DIV_32 0x50
#define SI_R_DIV_64 0x60
#define SI_R_DIV_128 0x70

#define SI_CLK_SRC_PLL_A 0x00
#define SI_CLK_SRC_PLL_B 0x20

#define XTAL_FREQ 27000000 // Crystal frequency

void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency(uint32_t frequency);

#endif
