#ifndef HcalUpgradeTriggerPrimitiveDigi_H
#define HcalUpgradeTriggerPrimitiveDigi_H 1

#include <ostream>
#include <vector>
#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/HcalDigi/interface/HcalUpgradeTriggerPrimitiveSample.h"

/** \class HcalUpgradeTriggerPrimitiveDigi
    
\author J. Mans - Minnesota
*/
class HcalUpgradeTriggerPrimitiveDigi {
public:
  typedef HcalTrigTowerDetId key_type; ///< For the sorted collection

  HcalUpgradeTriggerPrimitiveDigi(); // for persistence
  explicit HcalUpgradeTriggerPrimitiveDigi(const HcalTrigTowerDetId& id);
  
  const HcalTrigTowerDetId& id() const { return id_; }
  int size() const { return (size_&0xF); }
  int presamples() const { return hcalPresamples_&0xF; }

   /// was ZS MarkAndPass?
  bool zsMarkAndPass() const { return (hcalPresamples_&0x10); }
  /// was ZS unsuppressed?
  bool zsUnsuppressed() const { return (hcalPresamples_&0x20); }

  void setZSInfo(bool unsuppressed, bool markAndPass);

  const HcalUpgradeTriggerPrimitiveSample& operator[](int i) const { return data_[i]; }
  const HcalUpgradeTriggerPrimitiveSample& sample(int i) const { return data_[i]; }

  /// Full "Sample of Interest"
  const HcalUpgradeTriggerPrimitiveSample& t0() const { return data_[presamples()]; }  
  /// Fine-grain bit for the "Sample of Interest"
  bool SOI_fineGrain(int i=0) const { return t0().fineGrain(i); }
  /// Compressed ET for the "Sample of Interest"
  int SOI_compressedEt() const { return t0().compressedEt(); }

  void setSize(int size);
  void setPresamples(int ps);
  void setSample(int i, const HcalUpgradeTriggerPrimitiveSample& sam) { data_[i]=sam; }
  
  static const int MAXSAMPLES = 10;
private:
  HcalTrigTowerDetId id_;
  int size_;
  int hcalPresamples_;
  HcalUpgradeTriggerPrimitiveSample data_[MAXSAMPLES];
};

std::ostream& operator<<(std::ostream& s, const HcalUpgradeTriggerPrimitiveDigi& digi);


#endif
