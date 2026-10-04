# *This project was worked on under a template repository. I have moved the finished project here and made it publicly available*
# LACC Student Starter (Phase 1.4)

This repository is the reduced student starter for the LACC course project.

It is intentionally designed to compile before the core student-owned pieces are
implemented. The Catch2 tests define the authoritative behavioral contract.
Students validate their work by passing the provided test suite.

## Student-owned implementation areas

Students are expected to complete:
- `PriorityQueue` (binary heap ADT)
- `HashTableChained` (separate chaining hash table ADT)
- plain-text file reading helper
- DNA FASTA-like parser
- GW CSV parser
- DNA top-k k-mer analysis
- GW matched-filter peak analysis

## Instructor-provided framework

The repository still provides:
- CMake build system
- Catch2 test wiring
- CLI/framework scaffolding
- MPI/OpenMP support scaffolding
- plotting support
- demo fixture data

## Build

```bash
mkdir -p build && cd build
cmake ..
cmake --build . -j
ctest --output-on-failure
```

## Run the app

```bash
./lacc_analyze gw ../data/gw_sample.csv ./gw_out
./lacc_analyze dna ../data/dna_sample.fasta ./dna_out
```

At the start of the lab, tests and app runs will fail until the required
student functions are implemented.

## Testing and validation

This project uses Catch2 as the authoritative validation system.

Passing the tests demonstrates that an implementation satisfies the contract for:

- ADT correctness
- input parsing
- artifact extraction logic
- output formatting and deterministic ordering

Important:

- tests validate behavior on controlled fixtures
- different valid inputs will produce different valid outputs
- the same input must produce the same output

## DNA input format for this starter

The supplied DNA fixtures use a FASTA-like format:

```text
>read_001
ACTGACTG
>read_002
TTTTGGGA
```

See `docs/DNA_Input_Contract.md` for the authoritative starter contract.
