#include "softrock.h"
#include "iq_audio_source.h"

Softrock::Softrock() : rig(RIG_MODEL_SI570AVRUSB) { rig.open(); }

gr::basic_block_sptr Softrock::source() { return iq_audio_source::make(); }

void Softrock::set_freq(double freq) { rig.setFreq(freq); }
