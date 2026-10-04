#include <catch2/catch_test_macros.hpp>

#include "lacc/bio/Fasta.hpp"
#include "lacc/bio/Kmer.hpp"

#include <fstream>
#include <string>
#include <vector>

TEST_CASE("FASTA-like parser reads multiple records and multiline sequences") {
  const std::string path = "tmp_test_reads.fasta";

  {
    std::ofstream out(path);
    REQUIRE(out.good());
    out << ">read_001\n";
    out << "ACTG\n";
    out << "ACTG\n";
    out << ">read_002\n";
    out << "TTAA\n";
  }

  const auto reads = lacc::bio::read_fasta_like(path);
  REQUIRE(reads.size() == 2);

  REQUIRE(reads[0].id == "read_001");
  REQUIRE(reads[0].seq == "ACTGACTG");

  REQUIRE(reads[1].id == "read_002");
  REQUIRE(reads[1].seq == "TTAA");
}

TEST_CASE("FASTA-like parser throws on malformed input before first header") {
  const std::string path = "tmp_test_bad_reads.fasta";

  {
    std::ofstream out(path);
    REQUIRE(out.good());
    out << "ACTG\n";
    out << ">read_001\n";
    out << "TTAA\n";
  }

  REQUIRE_THROWS_AS(
    lacc::bio::read_fasta_like(path),
    std::runtime_error
  );
}

TEST_CASE("DNA fixture: top-10 3-mers are deterministic for dna_sample.fasta") {
  const std::string path = "data/dna_sample.fasta";
  auto reads = lacc::bio::read_fasta_like(path);

  std::vector<std::string> seqs;
  seqs.reserve(reads.size());
  for (const auto& r : reads) {
    seqs.push_back(r.seq);
  }

  const auto tops = lacc::bio::count_kmers_topk(seqs, /*k=*/3, /*topk=*/10);

  REQUIRE(tops.size() == 10);

  const std::vector<lacc::bio::KmerTop> expected = {
    {"ACG", 3},
    {"CGT", 3},
    {"GTA", 2},
    {"TAG", 2},
    {"AAC", 1},
    {"AGC", 1},
    {"CAA", 1},
    {"GCA", 1},
    {"GTG", 1},
    {"GTT", 1},
  };

  for (std::size_t i = 0; i < expected.size(); ++i) {
    INFO("index=" << i);
    REQUIRE(tops[i].kmer == expected[i].kmer);
    REQUIRE(tops[i].count == expected[i].count);
  }
}

TEST_CASE("DNA k-mer results are ordered deterministically") {
  const std::vector<std::string> seqs = {
    "AAAA",
    "AAAT"
  };

  const auto tops = lacc::bio::count_kmers_topk(seqs, /*k=*/2, /*topk=*/3);

  REQUIRE(tops.size() == 2);
  REQUIRE(tops[0].kmer == "AA");
  REQUIRE(tops[0].count == 5);
  REQUIRE(tops[1].kmer == "AT");
  REQUIRE(tops[1].count == 1);
}
