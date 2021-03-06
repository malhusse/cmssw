#ifndef RecoMuon_CosmicMuonProducer_CosmicMuonProducer_H
#define RecoMuon_CosmicMuonProducer_CosmicMuonProducer_H

/** \file CosmicMuonProducer
 *
 *  \author Chang Liu
 */

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"

class MuonTrackFinder;
class MuonServiceProxy;

class CosmicMuonProducer : public edm::stream::EDProducer<> {
public:
  explicit CosmicMuonProducer(const edm::ParameterSet&);

  ~CosmicMuonProducer() override;

  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  MuonTrackFinder* theTrackFinder;

  /// the event setup proxy, it takes care the services update
  MuonServiceProxy* theService;
  edm::EDGetTokenT<edm::View<TrajectorySeed> > theSeedCollectionToken;
};

#endif
