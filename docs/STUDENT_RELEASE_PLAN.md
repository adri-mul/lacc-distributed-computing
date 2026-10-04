# Student Release Plan (Phase 1.4)

## Purpose
This student starter is a reduced release of the LACC framework. It preserves
build, test, CLI, and scaffolding behavior while removing completed
implementations from the main student learning objectives.

## Student-owned files
- `include/lacc/adt/PriorityQueue.hpp`
- `src/adt/HashTableChained.cpp`
- `src/io/FileReader.cpp`
- `src/bio/Fasta.cpp`
- `src/physics/GwCsv.cpp`
- `src/bio/Kmer.cpp`
- `src/physics/MatchedFilter.cpp`

## Design goals
- compile immediately
- fail clearly where student work is missing
- embed input-format guidance directly in parser stubs
- preserve public interfaces for grading and later extension

## Validation model
Catch2 tests are the authoritative validation layer for student implementations.
The tests define the behavior students must satisfy.

## Notes
- DNA fixtures in this starter are FASTA-like.
- GW fixtures in this starter are CSV with `time,strain` columns.
- Serial correctness and deterministic output come before parallel speedups.
