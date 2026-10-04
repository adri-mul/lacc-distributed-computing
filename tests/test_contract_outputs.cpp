#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

static bool file_exists_and_nonempty(const std::string& path) {
  std::ifstream in(path);
  if (!in) return false;
  std::string first_line;
  return static_cast<bool>(std::getline(in, first_line));
}

static std::string first_line_of(const std::string& path) {
  std::ifstream in(path);
  if (!in) return "";
  std::string line;
  std::getline(in, line);
  return line;
}

int main() {
  {
    const int rc = std::system("./lacc_analyze gw data/gw_sample.csv contract_gw");
    if (rc != 0) {
      std::cerr << "GW contract run failed\n";
      return 1;
    }
    if (!file_exists_and_nonempty("contract_gw_peaks.csv")) {
      std::cerr << "GW output file missing or empty\n";
      return 2;
    }
    if (first_line_of("contract_gw_peaks.csv") != "time,score") {
      std::cerr << "GW output header mismatch\n";
      return 3;
    }
  }

  {
    const int rc = std::system("./lacc_analyze dna data/dna_sample.fasta contract_dna");
    if (rc != 0) {
      std::cerr << "DNA contract run failed\n";
      return 4;
    }
    if (!file_exists_and_nonempty("contract_dna_kmers.csv")) {
      std::cerr << "DNA output file missing or empty\n";
      return 5;
    }
    if (first_line_of("contract_dna_kmers.csv") != "kmer,count") {
      std::cerr << "DNA output header mismatch\n";
      return 6;
    }
  }

  std::cout << "test_contract_outputs OK\n";
  return 0;
}
