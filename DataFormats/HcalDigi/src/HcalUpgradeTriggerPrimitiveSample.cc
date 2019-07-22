#include "DataFormats/HcalDigi/interface/HcalUpgradeTriggerPrimitiveSample.h"


HcalUpgradeTriggerPrimitiveSample::HcalUpgradeTriggerPrimitiveSample() : theSample(0) { }
HcalUpgradeTriggerPrimitiveSample::HcalUpgradeTriggerPrimitiveSample(uint16_t data) : theSample(data) { }

HcalUpgradeTriggerPrimitiveSample::HcalUpgradeTriggerPrimitiveSample(int encodedEt, bool fineGrain, int slb, int slbchan) { 
  theSample=(((slb)&0x7)<<13)|((slbchan&0x3)<<11)|(encodedEt&0xFF)|
    ((fineGrain)?(0x100):(0));
}

HcalUpgradeTriggerPrimitiveSample::HcalUpgradeTriggerPrimitiveSample(int encodedEt, int fineGrainExt) {
  theSample=(encodedEt&0xFF)|((fineGrainExt&0x3F)<<8);
}

HcalUpgradeTriggerPrimitiveSample::HcalUpgradeTriggerPrimitiveSample(int encodedEt, int fineGrainExt, std::pair<float, float> shortLongEns) {
  theSample=(encodedEt&0xFF)|((fineGrainExt&0x3F)<<8);
  shortLongEnergies_ = shortLongEns;
}

std::ostream& operator<<(std::ostream& s, const HcalUpgradeTriggerPrimitiveSample& samp) {
  return s << "Value=" << samp.compressedEt() << ", FG=" << samp.fineGrain();
}


