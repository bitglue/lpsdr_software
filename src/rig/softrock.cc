#include "softrock.h"
#include "iq_audio_source.h"

Softrock::Softrock(unsigned sample_rate)
    : IQOnly(sample_rate), rig(RIG_MODEL_SI570AVRUSB) {
  rig.open();
}

void Softrock::set_freq(double freq) { rig.setFreq(freq); }
double Softrock::get_freq() { return rig.getFreq(); }
