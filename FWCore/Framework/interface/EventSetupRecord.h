#ifndef FWCore_Framework_EventSetupRecord_h
#define FWCore_Framework_EventSetupRecord_h
// -*- C++ -*-
//
// Package:     Framework
// Class  :     EventSetupRecord
//
/**\class EventSetupRecord EventSetupRecord.h FWCore/Framework/interface/EventSetupRecord.h

 Description: Base class for all Records in an EventSetup.  Holds data with the same lifetime.

 Usage:
This class contains the Proxies that make up a given Record.  It
is designed to be reused time after time, rather than it being
destroyed and a new one created every time a new Record is
required.  Proxies can only be added by the EventSetupRecordProvider class which
uses the 'add' function to do this.  The reason for this is
that the EventSetupRecordProvider/DataProxyProvider pair are responsible for
"invalidating" Proxies in a Record.  When a Record
becomes "invalid" the EventSetupRecordProvider must invalidate
all the  Proxies which it does using the DataProxyProvider.

When the set of  Proxies for a Records changes, i.e. a
DataProxyProvider is added of removed from the system, then the
Proxies in a Record need to be changes as appropriate.
In this design it was decided the easiest way to achieve this was
to erase all  Proxies in a Record.

It is important for the management of the Records that each Record
know the ValidityInterval that represents the time over which its data is valid.
The ValidityInterval is set by its EventSetupRecordProvider using the
'set' function.  This quantity can be recovered
through the 'validityInterval' method.

For a Proxy to be able to derive its contents from the EventSetup, it
must be able to access any Proxy (and thus any Record) in the
EventSetup.  The 'make' function of a Proxy provides its
containing Record as one of its arguments.  To be able to
access the rest of the EventSetup, it is necessary for a Record to be
able to access its containing EventSetup.  This task is handled by the
'eventSetup' function.  The EventSetup is responsible for managing this
using the 'setEventSetup' and 'clearEventSetup' functions.

*/
//
// Author:      Chris Jones
// Created:     Fri Mar 25 14:38:35 EST 2005
//

// user include files
#include "FWCore/Framework/interface/FunctorESHandleExceptionFactory.h"
#include "FWCore/Framework/interface/DataKey.h"
#include "FWCore/Framework/interface/NoProxyException.h"
#include "FWCore/Framework/interface/ValidityInterval.h"
#include "FWCore/Framework/interface/EventSetupRecordImpl.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Utilities/interface/ESInputTag.h"
#include "FWCore/Utilities/interface/ESIndices.h"
#include "FWCore/Utilities/interface/Likely.h"

// system include files
#include <exception>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <atomic>
#include <cassert>
#include <limits>

// forward declarations
namespace cms {
  class Exception;
}

class testEventsetup;
class testEventsetupRecord;

namespace edm {
  template <typename T>
  class ESHandle;
  class ESHandleExceptionFactory;
  class ESInputTag;
  class EventSetupImpl;

  namespace eventsetup {
    struct ComponentDescription;
    class DataProxy;
    class EventSetupRecordKey;

    class EventSetupRecord {
      friend class ::testEventsetup;
      friend class ::testEventsetupRecord;

    public:
      EventSetupRecord();
      EventSetupRecord(EventSetupRecord&&) = default;
      EventSetupRecord& operator=(EventSetupRecord&&) = default;

      EventSetupRecord(EventSetupRecord const&) = default;
      EventSetupRecord& operator=(EventSetupRecord const&) = default;
      virtual ~EventSetupRecord();

      // ---------- const member functions ---------------------
      ValidityInterval const& validityInterval() const { return impl_->validityInterval(); }

      void setImpl(EventSetupRecordImpl const* iImpl, unsigned int transitionID, ESProxyIndex const* getTokenIndices) {
        impl_ = iImpl;
        transitionID_ = transitionID;
        getTokenIndices_ = getTokenIndices;
      }

      template <typename HolderT>
      bool get(HolderT& iHolder) const {
        return get("", iHolder);
      }

      template <typename HolderT>
      bool get(char const* iName, HolderT& iHolder) const {
        typename HolderT::value_type const* value = nullptr;
        ComponentDescription const* desc = nullptr;
        std::shared_ptr<ESHandleExceptionFactory> whyFailedFactory;
        impl_->getImplementation(value, iName, desc, iHolder.transientAccessOnly, whyFailedFactory);

        if (value) {
          iHolder = HolderT(value, desc);
          return true;
        } else {
          iHolder = HolderT(std::move(whyFailedFactory));
          return false;
        }
      }
      template <typename HolderT>
      bool get(std::string const& iName, HolderT& iHolder) const {
        return get(iName.c_str(), iHolder);
      }

      template <typename HolderT>
      bool get(ESInputTag const& iTag, HolderT& iHolder) const {
        typename HolderT::value_type const* value = nullptr;
        ComponentDescription const* desc = nullptr;
        std::shared_ptr<ESHandleExceptionFactory> whyFailedFactory;
        impl_->getImplementation(value, iTag.data().c_str(), desc, iHolder.transientAccessOnly, whyFailedFactory);

        if (value) {
          validate(desc, iTag);
          iHolder = HolderT(value, desc);
          return true;
        } else {
          iHolder = HolderT(std::move(whyFailedFactory));
          return false;
        }
      }

      ///returns false if no data available for key
      bool doGet(DataKey const& aKey, bool aGetTransiently = false) const;

      /**returns true only if someone has already requested data for this key
          and the data was retrieved
          */
      bool wasGotten(DataKey const& aKey) const;

      /**returns the ComponentDescription for the module which creates the data or 0
          if no module has been registered for the data. This does not cause the data to
          actually be constructed.
          */
      ComponentDescription const* providerDescription(DataKey const& aKey) const;

      virtual EventSetupRecordKey key() const = 0;

      /**If you are caching data from the Record, you should also keep
          this number.  If this number changes then you know that
          the data you have cached is invalid. This is NOT true if
          if the validityInterval() hasn't changed since it is possible that
          the job has gone to a new Record and then come back to the
          previous SyncValue and your algorithm didn't see the intervening
          Record.
          The value of '0' will never be returned so you can use that to
          denote that you have not yet checked the value.
          */
      unsigned long long cacheIdentifier() const { return impl_->cacheIdentifier(); }

      ///clears the oToFill vector and then fills it with the keys for all registered data keys
      void fillRegisteredDataKeys(std::vector<DataKey>& oToFill) const { impl_->fillRegisteredDataKeys(oToFill); }

    protected:
      template <template <typename> typename H, typename T, typename R>
      H<T> getHandleImpl(ESGetToken<T, R> const& iToken) const {
        assert(iToken.transitionID() == transitionID());
        assert(iToken.isInitialized());
        assert(getTokenIndices_);
        //need to check token has valid index
        if
          UNLIKELY(not iToken.hasValidIndex()) { return invalidTokenHandle<H>(iToken); }

        auto proxyIndex = getTokenIndices_[iToken.index().value()];
        if
          UNLIKELY(proxyIndex.value() == std::numeric_limits<int>::max()) { return noProxyHandle<H>(iToken); }

        T const* value = nullptr;
        ComponentDescription const* desc = nullptr;
        std::shared_ptr<ESHandleExceptionFactory> whyFailedFactory;
        impl_->getImplementation(value, proxyIndex, H<T>::transientAccessOnly, desc, whyFailedFactory);

        if
          UNLIKELY(not value) { return H<T>(std::move(whyFailedFactory)); }
        return H<T>(value, desc);
      }

      DataProxy const* find(DataKey const& aKey) const;

      EventSetupImpl const& eventSetup() const { return impl_->eventSetup(); }

      ESProxyIndex const* getTokenIndices() const { return getTokenIndices_; }

      void validate(ComponentDescription const*, ESInputTag const&) const;

      void addTraceInfoToCmsException(cms::Exception& iException,
                                      char const* iName,
                                      ComponentDescription const*,
                                      DataKey const&) const;
      void changeStdExceptionToCmsException(char const* iExceptionWhatMessage,
                                            char const* iName,
                                            ComponentDescription const*,
                                            DataKey const&) const;

      EventSetupRecordImpl const* impl() const { return impl_; }

      unsigned int transitionID() const { return transitionID_; }

    private:
      template <template <typename> typename H, typename T, typename R>
      H<T> invalidTokenHandle(ESGetToken<T, R> const& iToken) const {
        auto const key = this->key();
        return H<T>{
            makeESHandleExceptionFactory([key] { return makeInvalidTokenException(key, DataKey::makeTypeTag<T>()); })};
      }

      template <template <typename> typename H, typename T, typename R>
      H<T> noProxyHandle(ESGetToken<T, R> const& iToken) const {
        auto const key = this->key();
        auto name = iToken.name();
        return H<T>{makeESHandleExceptionFactory([key, name] {
          NoProxyException<T> ex(key, DataKey{DataKey::makeTypeTag<T>(), name});
          return std::make_exception_ptr(ex);
        })};
      }

      void const* getFromProxy(DataKey const& iKey,
                               ComponentDescription const*& iDesc,
                               bool iTransientAccessOnly) const;

      static std::exception_ptr makeInvalidTokenException(EventSetupRecordKey const&, TypeTag const&);
      // ---------- member data --------------------------------
      EventSetupRecordImpl const* impl_ = nullptr;
      ESProxyIndex const* getTokenIndices_ = nullptr;
      unsigned int transitionID_ = std::numeric_limits<unsigned int>::max();
    };

    class EventSetupRecordGeneric : public EventSetupRecord {
    public:
      EventSetupRecordGeneric(EventSetupRecordImpl const* iImpl,
                              unsigned int iTransitionID,
                              ESProxyIndex const* getTokenIndices) {
        setImpl(iImpl, iTransitionID, getTokenIndices);
      }

      EventSetupRecordKey key() const final { return impl()->key(); }
    };
  }  // namespace eventsetup
}  // namespace edm
#endif
