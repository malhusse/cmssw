#include "FWCore/Services/src/SiteLocalConfigService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Test SiteLocalConfigService", "[sitelocalconfig]") {
  std::string dirString;
  auto dir = getenv("LOCAL_TEST_DIR");
  if (dir) {
    dirString = dir;
  } else {
    auto dir = getenv("CMSSW_BASE");
    if (dir) {
      dirString = dir;
      dirString += "/src/FWCore/Services/test";
    }
  }
  REQUIRE(not dirString.empty());
  SECTION("full-site-local-config.testfile") {
    edm::ParameterSet pset;
    pset.addUntrackedParameter<std::string>("siteLocalConfigFileUrl", dirString + "/full-site-local-config.testfile");

    edm::service::SiteLocalConfigService slc(pset);

    CHECK(slc.dataCatalog() == "trivialcatalog_file:/dummy/storage.xml?protocol=dcap");
    CHECK(slc.fallbackDataCatalog().empty());
    REQUIRE(slc.sourceCacheTempDir() != nullptr);
    CHECK(*slc.sourceCacheTempDir() == "/a/b/c");
    CHECK(slc.sourceCacheMinFree() == nullptr);
    REQUIRE(slc.sourceCacheHint() != nullptr);
    CHECK(*slc.sourceCacheHint() == "application-only");
    REQUIRE(slc.sourceCloneCacheHint() == nullptr);
    REQUIRE(slc.sourceReadHint() != nullptr);
    CHECK(*slc.sourceReadHint() == "read-ahead-buffered");
    REQUIRE(slc.sourceTTreeCacheSize() != nullptr);
    CHECK(*slc.sourceTTreeCacheSize() == 10000);
    REQUIRE(slc.sourceTimeout() != nullptr);
    CHECK(*slc.sourceTimeout() == 7200);
    CHECK(slc.enablePrefetching());
    REQUIRE(slc.sourceNativeProtocols() != nullptr);
    REQUIRE(slc.sourceNativeProtocols()->size() == 2);
    CHECK((*slc.sourceNativeProtocols())[0] == "dcache");
    CHECK((*slc.sourceNativeProtocols())[1] == "file");
    REQUIRE(slc.statisticsDestination() != nullptr);
    REQUIRE(slc.statisticsInfo() != nullptr);
    CHECK(slc.statisticsInfo()->size() == 1);
    CHECK(slc.statisticsInfo()->find("dn") != slc.statisticsInfo()->end());
    CHECK(slc.siteName() == "DUMMY");
  }

  SECTION("overrides") {
    edm::ParameterSet pset;
    pset.addUntrackedParameter<std::string>("siteLocalConfigFileUrl", dirString + "/full-site-local-config.testfile");

    pset.addUntrackedParameter<std::string>("overrideSourceCacheTempDir", "/a/d");
    pset.addUntrackedParameter<double>("overrideSourceCacheMinFree", 10.);
    pset.addUntrackedParameter<std::string>("overrideSourceCacheHintDir", "dummy");
    pset.addUntrackedParameter<std::string>("overrideSourceCloneCacheHintDir", "foo");
    pset.addUntrackedParameter<std::string>("overrideSourceReadHint", "read-ahead-unbuffered");
    pset.addUntrackedParameter<std::vector<std::string> >("overrideSourceNativeProtocols", {{"http"}});
    pset.addUntrackedParameter<unsigned int>("overrideSourceTTreeCacheSize", 20000);
    pset.addUntrackedParameter<unsigned int>("overrideSourceTimeout", 0);
    pset.addUntrackedParameter<bool>("overridePrefetching", false);
    pset.addUntrackedParameter<std::string>("overrideStatisticsDestination", "");
    pset.addUntrackedParameter<std::vector<std::string> >("overrideStatisticsInfo", {{"nodn"}});

    edm::service::SiteLocalConfigService slc(pset);

    CHECK(slc.dataCatalog() == "trivialcatalog_file:/dummy/storage.xml?protocol=dcap");
    CHECK(slc.fallbackDataCatalog().empty());
    REQUIRE(slc.sourceCacheTempDir() != nullptr);
    CHECK(*slc.sourceCacheTempDir() == "/a/d");
    REQUIRE(slc.sourceCacheMinFree() != nullptr);
    CHECK(*slc.sourceCacheMinFree() == 10.);
    REQUIRE(slc.sourceCacheHint() != nullptr);
    CHECK(*slc.sourceCacheHint() == "dummy");
    REQUIRE(slc.sourceCloneCacheHint() != nullptr);
    CHECK(*slc.sourceCloneCacheHint() == "foo");
    REQUIRE(slc.sourceReadHint() != nullptr);
    CHECK(*slc.sourceReadHint() == "read-ahead-unbuffered");
    REQUIRE(slc.sourceTTreeCacheSize() != nullptr);
    CHECK(*slc.sourceTTreeCacheSize() == 20000);
    REQUIRE(slc.sourceTimeout() != nullptr);
    CHECK(*slc.sourceTimeout() == 0);
    CHECK(not slc.enablePrefetching());
    REQUIRE(slc.sourceNativeProtocols() != nullptr);
    REQUIRE(slc.sourceNativeProtocols()->size() == 1);
    CHECK((*slc.sourceNativeProtocols())[0] == "http");
    REQUIRE(slc.statisticsDestination() == nullptr);
    REQUIRE(slc.statisticsInfo() != nullptr);
    CHECK(slc.statisticsInfo()->size() == 1);
    CHECK(slc.statisticsInfo()->find("nodn") != slc.statisticsInfo()->end());
    CHECK(slc.siteName() == "DUMMY");
  }
}
