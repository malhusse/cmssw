#ifndef HcalUpgradeTriggerPrimitiveSample_H
#define HcalUpgradeTriggerPrimitiveSample_H 1

#include <boost/cstdint.hpp>
#include <ostream>

/** \class HcalUpgradeTriggerPrimitiveSample
    
  \author J. Mans - Minnesota
*/
class HcalUpgradeTriggerPrimitiveSample {
public:
  HcalUpgradeTriggerPrimitiveSample();
  HcalUpgradeTriggerPrimitiveSample(uint16_t data);
  HcalUpgradeTriggerPrimitiveSample(int encodedEt, bool finegrain, int slb, int slbchan);
  HcalUpgradeTriggerPrimitiveSample(int encodedEt, int finegrainExtended);
  HcalUpgradeTriggerPrimitiveSample(int encodedEt, int finegrainExtended, std::pair<float, float> shortLongEns);
  
  /// get the raw word
  uint16_t raw() const { return theSample; }
  /// get the encoded/compressed Et
  int compressedEt() const { return theSample&0xFF; }
  /// get fine-grain bit (traditional)
  bool fineGrain(int i=0) const { return (((theSample)>>(i+8))&0x1)!=0; }
  std::pair<float,float> getShortLongEns() const { return shortLongEnergies_;}

private:
  uint16_t theSample;
  std::pair <float,float> shortLongEnergies_;
};

std::ostream& operator<<(std::ostream& s, const HcalUpgradeTriggerPrimitiveSample& samp);


#endif
