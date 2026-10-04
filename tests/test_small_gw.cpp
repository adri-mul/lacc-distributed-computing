#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "lacc/physics/GwCsv.hpp"
#include "lacc/physics/MatchedFilter.hpp"

#include <fstream>
#include <string>
#include <vector>

TEST_CASE("GW CSV parser reads headered two-column files") {
  const std::string path = "tmp_test_gw.csv";

  {
    std::ofstream out(path);
    REQUIRE(out.good());
    out << "time,strain\n";
    out << "0.0,0.01\n";
    out << "0.1,0.04\n";
    out << "0.2,-0.02\n";
  }

  const auto series = lacc::physics::read_gw_csv(path);
  REQUIRE(series.t.size() == 3);
  REQUIRE(series.strain.size() == 3);

  REQUIRE(series.t[0] == Catch::Approx(0.0));
  REQUIRE(series.t[1] == Catch::Approx(0.1));
  REQUIRE(series.t[2] == Catch::Approx(0.2));

  REQUIRE(series.strain[0] == Catch::Approx(0.01));
  REQUIRE(series.strain[1] == Catch::Approx(0.04));
  REQUIRE(series.strain[2] == Catch::Approx(-0.02));
}

TEST_CASE("GW CSV parser throws on malformed rows") {
  const std::string path = "tmp_test_bad_gw.csv";

  {
    std::ofstream out(path);
    REQUIRE(out.good());
    out << "time,strain\n";
    out << "0.0\n";
  }

  REQUIRE_THROWS_AS(
    lacc::physics::read_gw_csv(path),
    std::runtime_error
  );
}

TEST_CASE("GW fixture: matched-filter peaks are deterministic for gw_sample.csv") {
  const std::string path = "data/gw_sample.csv";
  const auto series = lacc::physics::read_gw_csv(path);

  const auto peaks = lacc::physics::simple_matched_filter_peaks(
    series.t,
    series.strain,
    /*window=*/4,
    /*threshold=*/0.25,
    /*topk=*/5
  );

  REQUIRE(peaks.size() == 1);
  REQUIRE(peaks[0].time == Catch::Approx(0.2));
  REQUIRE(peaks[0].score == Catch::Approx(0.4666666667).epsilon(1e-6));
}

TEST_CASE("GW peak ordering is deterministic for tied or near-tied candidates") {
  const std::vector<double> t = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5};
  const std::vector<double> strain = {0.0, 1.0, 0.0, 1.0, 0.0, 0.0};

  const auto peaks = lacc::physics::simple_matched_filter_peaks(
    t, strain, /*window=*/3, /*threshold=*/0.1, /*topk=*/5
  );

  REQUIRE(peaks.size() >= 1);

  for (std::size_t i = 1; i < peaks.size(); ++i) {
    const bool ordered =
      (peaks[i - 1].score > peaks[i].score) ||
      ((peaks[i - 1].score == peaks[i].score) && (peaks[i - 1].time <= peaks[i].time));

    REQUIRE(ordered);
  }
}
