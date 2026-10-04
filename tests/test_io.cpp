#include <catch2/catch_test_macros.hpp>

#include "lacc/io/FileReader.hpp"

#include <fstream>
#include <string>
#include <vector>

TEST_CASE("read_all_lines returns file lines in order and preserves blanks") {
  const std::string path = "tmp_test_io_lines.txt";

  {
    std::ofstream out(path);
    REQUIRE(out.good());
    out << "alpha\n";
    out << "\n";
    out << "gamma\n";
  }

  const auto lines = lacc::io::read_all_lines(path);
  REQUIRE(lines.size() == 3);
  REQUIRE(lines[0] == "alpha");
  REQUIRE(lines[1] == "");
  REQUIRE(lines[2] == "gamma");
}

TEST_CASE("read_all_lines throws on missing file") {
  REQUIRE_THROWS_AS(
    lacc::io::read_all_lines("definitely_missing_input_file.txt"),
    std::runtime_error
  );
}
