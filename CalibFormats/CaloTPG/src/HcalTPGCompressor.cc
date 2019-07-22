#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"
#include "CalibFormats/CaloTPG/interface/HcalTPGCompressor.h"

HcalTPGCompressor::HcalTPGCompressor(const CaloTPGTranscoder* coder) : coder_(coder) {
}
  
void HcalTPGCompressor::compress(const IntegerCaloSamples& ics, const std::vector<int>& fineGrain, HcalTriggerPrimitiveDigi& digi) const {
  digi.setSize(ics.size());
  digi.setPresamples(ics.presamples());
  for (int i=0; i<ics.size(); i++)
    digi.setSample(i,coder_->hcalCompress(ics.id(),ics[i],fineGrain[i]));
}

void HcalTPGCompressor::compress(const IntegerCaloSamples& ics, const std::vector<int>& fineGrain, HcalUpgradeTriggerPrimitiveDigi& digi) const {
  digi.setSize(ics.size());
  digi.setPresamples(ics.presamples());
  for (int i=0; i<ics.size(); i++)
    digi.setSample(i,coder_->hcalUpgradeCompress(ics.id(),ics[i],fineGrain[i],std::make_pair(0.0,0.0)));
}

void HcalTPGCompressor::compress(const IntegerCaloSamples& ics, const std::vector<int>& fineGrain, HcalUpgradeTriggerPrimitiveDigi& digi, std::vector<std::pair<float,float>>& shortLongEnergies) const {
  digi.setSize(ics.size());
  digi.setPresamples(ics.presamples());
  for (int i=0; i<ics.size(); i++)
    digi.setSample(i,coder_->hcalUpgradeCompress(ics.id(),ics[i],fineGrain[i],shortLongEnergies[i]));
}

HcalTriggerPrimitiveSample HcalTPGCompressor::compress(const HcalTrigTowerDetId& id, unsigned int sample, bool fineGrain) const {
  return coder_->hcalCompress(id, sample, fineGrain);
}
