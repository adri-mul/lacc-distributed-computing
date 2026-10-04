# DNA Input Contract
## LACC Environmental Sequencing Analysis

## 1. Purpose

This document defines the authoritative input contract for the DNA analysis tasks
in the Phase 1.4 student starter.

For this starter release, the supplied fixtures and starter parser target a
small FASTA-like text format, not FASTQ.

## 2. Input Data Format

### 2.1 FASTA-like Files

Input data consists of one or more plain-text files.

Each record begins with a header line:

    >read_id

The header is followed by one or more sequence lines:

    >read_001
    ACTGACTG
    ACTG
    >read_002
    TTTTGGGA

Where:
- `read_id` is an arbitrary identifier
- sequence lines contain characters such as `A`, `C`, `G`, `T`, or `N`
- all sequence lines after a header belong to that record until the next header

Blank lines may be ignored.

## 3. Valid Assumptions

Programs may assume:
- input files are plain text
- sequence lines contain valid nucleotide characters
- files are accessible through the normal filesystem

Programs must not assume:
- a fixed read length
- a fixed number of reads
- a fixed order of records beyond file order

## 4. K-mer Extraction Contract

- A fixed integer `k` is provided at runtime.
- Programs must extract all overlapping k-mers using a sliding window.
- Example with `k = 4`:

    ACTGAC -> ACTG, CTGA, TGAC

Rules:
- k-mers containing `N` may be discarded or handled consistently
- extraction must be deterministic
- no probabilistic filtering is permitted

## 5. Parallel Execution Assumptions

Programs must be safe to execute under MPI and OpenMP eventually, but the
student starter is graded first on serial correctness and deterministic output.

## 6. Error Handling

Programs must:
- detect malformed FASTA-like records
- report missing or unreadable input files
- exit clearly on fatal input errors

Programs must not silently ignore malformed structure.

## 7. Summary

| Aspect | Requirement |
|---|---|
| Input format | FASTA-like text |
| Record header | `>` followed by identifier |
| Sequence body | one or more subsequent lines |
| K-mer extraction | sliding window |
| Output | deterministic |
