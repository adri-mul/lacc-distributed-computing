#include "lacc/core/Cli.hpp"

#include <stdexcept>
#include <string>

namespace lacc::core {

static std::string usage() {
  return
      "Usage:\n"
      "  lacc_exec --domain <gw|dna> --outdir <DIR> [options]\n\n"
      "Options:\n"
      "  --topk <N>            Top-K results (default: 5)\n"
      "  --threads <N>         OpenMP threads hint (default: 1)\n"
      "  --distributed <mode>  off|auto|force (default: auto)\n"
      "  -h, --help            Show this help\n";
}

RunConfig::DistributedMode parse_distributed_mode(const std::string& s) {
  if (s == "off" || s == "0" || s == "false") return RunConfig::DistributedMode::Off;
  if (s == "auto") return RunConfig::DistributedMode::Auto;
  if (s == "force" || s == "on" || s == "1" || s == "true") return RunConfig::DistributedMode::Force;
  throw std::runtime_error("Unknown --distributed mode: '" + s + "'\n\n" + usage());
}

CliArgs parse_cli(int argc, char** argv) {
  CliArgs args;

  // Defaults are already set in the struct definitions.
  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i];

    if (a == "-h" || a == "--help") {
      throw std::runtime_error(usage());
    }

    auto require_value = [&](const char* flag) -> std::string {
      if (i + 1 >= argc) {
        throw std::runtime_error(std::string("Missing value for ") + flag + "\n\n" + usage());
      }
      return std::string(argv[++i]);
    };

    if (a == "--domain") {
      args.domain = require_value("--domain");
    } else if (a == "--outdir") {
      args.cfg.output_dir = require_value("--outdir");
    } else if (a == "--topk") {
      args.cfg.topk = std::stoi(require_value("--topk"));
    } else if (a == "--threads") {
      args.cfg.threads = std::stoi(require_value("--threads"));
    } else if (a == "--distributed") {
      args.cfg.distributed = parse_distributed_mode(require_value("--distributed"));
    } else {
      throw std::runtime_error("Unknown flag: " + a + "\n\n" + usage());
    }
  }

  if (args.domain.empty() || args.cfg.output_dir.empty()) {
    throw std::runtime_error("Missing required --domain and/or --outdir\n\n" + usage());
  }
  if (args.cfg.topk <= 0) {
    throw std::runtime_error("--topk must be >= 1\n\n" + usage());
  }
  if (args.cfg.threads <= 0) {
    throw std::runtime_error("--threads must be >= 1\n\n" + usage());
  }

  return args;
}

}  // namespace lacc::core
