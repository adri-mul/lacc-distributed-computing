# PROJECT_INTENT.md

## What is LACC?

LACC (Lightweight Analysis & Contracted Computing) is a **command-line scientific analysis project** used to teach data structures, algorithms, and systems thinking in a realistic software engineering context.

Students implement a single CLI application that performs domain-specific analyses (physics or biology) while adhering to a **strict external contract**: fixed command-line arguments, fixed output artifacts, and deterministic behavior verified by automated tests.

The project is intentionally framed as a *contracted deliverable* rather than a traditional homework assignment. The emphasis is on correctness, clarity, and reproducibility — not on performance optimization or advanced domain knowledge.

LACC is designed to be:
- **Cross-platform** (Linux, macOS, Windows)
- **IDE-agnostic**
- **Command-line driven**
- **Test-validated**

**Non-goal (scope control):** LACC is **not** intended to be a high-performance computing project or a domain research project; it is a software engineering exercise grounded in scientific context.

---

## What are the core ADTs?

The project deliberately centers on a small set of foundational Abstract Data Types that students are expected to understand and implement correctly.

### Core required ADTs
These are fundamental and remain conceptually central to both the DNA and Gravitational Wave tracks:

- **Linked List**
  - Used directly or indirectly (e.g., as the “buckets” in chained hashing)
  - Reinforces pointer-based structures, memory management, and ownership

- **Chained Hash Table**
  - The primary structure for counting and pattern lookup
  - Emphasizes hashing functions, collision resolution, and amortized O(1) behavior

- **Priority Queue (Binary Heap)**
  - Used to maintain Top-K results efficiently (e.g., most frequent DNA k-mers or strongest GW candidates)
  - Implemented as a binary heap over a dynamic array
  - Demonstrates partial ordering and O(N log K) time complexity for Top-K maintenance

Students are expected to understand the **invariants, time/space complexity, and common failure modes** of these ADTs — not just produce a working implementation.

These structures are intentionally reused across both tracks to emphasize the transferability of core CS ideas.

---

## What should never change?

The following are **contractual invariants**. They must remain stable even as internal implementations evolve.

### CLI contract
- The executable name and invocation style must remain stable.
- The program must accept the standardized argument set:

  `lacc_analyze --input <path> --mode <MODE> --k <K> --topk <N> --threshold <T> --outdir <DIR>`

- Argument names and meanings must not change.
- Each mode must **accept** all flags, even if some are unused in that mode (cross-compatibility requirement).

### Output contract
All runs must generate the required output artifacts inside `--outdir`:

- **summary.json**
- **candidates.csv**
- **histogram.csv**
- **run.log**

Additionally:
- CSV filenames and **CSV headers must match exactly**
- JSON keys and structure must match exactly
- Sorting and formatting rules must be deterministic (especially candidate sorting)

### Testing as authority
- Catch2-based tests define correctness.
- A solution that does not pass the automated test suite is not considered correct, even if it appears to “work” in a manual run.
- Tests are the arbiter, not IDE behavior or manual inspection.

---

## What is allowed to evolve?

The following are intentionally flexible and may be refactored by students:

- Internal algorithm choices (within the scope of the required ADTs)
- Helper utilities and internal abstractions (e.g., parsing helpers, small math utilities)
- Performance improvements that preserve behavioral stability and output determinism
- Additional unit tests (students are encouraged to add tests, but must not remove, weaken, or bypass existing tests)

As long as the external contract is honored and tests pass, internal code structure is free to change.

---

## Guidance for AI-assisted development (VS Code / Codex / Copilot)

When working with AI-assisted tools inside an IDE, always anchor the assistant to this document.

Use prompting language such as:

> “This project follows the design and constraints described in `docs/PROJECT_INTENT.md`.”

AI assistance is acceptable for:
- Explaining C++ syntax or error messages
- Suggesting refactors for existing code
- Debugging failing tests
- Clarifying data structure invariants (e.g., heapify-up / heapify-down correctness)

The developer is responsible for being able to **explain all submitted code and design choices**, regardless of the tools used.

AI assistance must not be used to bypass:
- the CLI contract,
- the required output formats,
- or the role of tests as the definition of truth.

---

## Summary

LACC is a contract-driven, test-validated, command-line project designed to teach core data structures in a realistic software engineering setting.

Stability of interfaces enables learning, refactoring, and experimentation — without breaking the system.

This document defines the intent.  
The tests define the truth.
