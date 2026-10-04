# MPI + OpenMP in LACC (Tutorial)

This tutorial is written for students who have **never used MPI or OpenMP** but are ready to read and modify code that *will* run on a distributed Linux cluster.

LACC uses a simple mental model:

- **MPI = multiple processes** (possibly on different nodes)
- **OpenMP = multiple threads** inside each process (shared memory)

In practice, you will often run **one MPI rank per node**, and **multiple OpenMP threads per rank**.

---

## Glossary

- **Rank**: an MPI process within an MPI job. Ranks are numbered `0..(size-1)`.
- **Size**: total number of ranks.
- **Communicator**: a group of ranks that can talk to each other. Most beginner programs use `MPI_COMM_WORLD` (all ranks).
- **Collective**: an MPI call that involves *all* ranks in the communicator (e.g., `MPI_Barrier`, `MPI_Bcast`, `MPI_Reduce`).

---

## 1) MPI lifecycle (the minimum you must know)

An MPI program follows this life cycle:

1. `MPI_Init(&argc, &argv)`
2. `MPI_Comm_rank(MPI_COMM_WORLD, &rank)`
3. `MPI_Comm_size(MPI_COMM_WORLD, &size)`
4. ... your program ...
5. `MPI_Finalize()`

In LACC, this is wrapped in `lacc::core::Mpi`:

- File: `include/lacc/core/Mpi.hpp`
- Marker: **[MPI-LIFECYCLE]**

Why wrap it?

- The project must compile **without** MPI installed.
- When MPI is available, the wrapper becomes a thin, explicit bridge to the real calls.

---

## 2) OpenMP basics

OpenMP is a compiler-supported threading model.

You will see it in code as:

- **Pragmas** like `#pragma omp parallel for`
- Runtime calls like `omp_get_max_threads()` or `omp_set_num_threads(n)`

In LACC, thread-count control is routed through the CLI and then applied in the runner:

- File: `src/core/Runner.cpp`
- Marker: **[OMP-THREADS]**

### Typical OpenMP patterns

Parallel-for loop:

```cpp
#pragma omp parallel for
for (int i = 0; i < N; ++i) {
  // work on i
}
```

Reducing a sum:

```cpp
double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
for (int i = 0; i < N; ++i) {
  sum += values[i];
}
```

Key idea:

- OpenMP is great inside a **single shared-memory node**.
- It does not replace MPI (which spans multiple nodes).

---

## 3) How LACC decides: serial vs distributed

LACC supports three modes:

- `--distributed auto` (default): use MPI distribution only when `size > 1`
- `--distributed on`: force the distributed code-path even if `size == 1`
- `--distributed off`: force a single-rank serial code-path (rank 0 does all work)

Implementation:

- File: `src/core/Runner.cpp`
- Function: `lacc::core::should_run_distributed(...)`
- Marker: **[MPI-DISTRIBUTED-MODE]**

Why have a flag?

- On a cluster, you might run the same binary in different ways:
  - interactive single node (debug)
  - batch multi-node (production)
- A flag makes the behavior explicit and testable.

---

## 4) Work distribution in LACC

LACC’s distributed work model is intentionally simple:

1. Rank 0 creates a *plan* describing which rank should process which items.
2. Rank 0 sends each rank its chunk.
3. Each rank processes its local chunk.
4. Results are gathered back to rank 0 and written to output.

The distribution layer is in:

- File: `src/core/WorkDistributorMPI.cpp`
- Marker: **[MPI-WORK-DISTRIBUTION]**

Right now, the distribution logic is minimal and heavily commented. That’s by design: it’s a teaching surface.

---

## 5) “Keep tests serial” (and what we do instead)

By default, `./build_and_run.sh` runs a **serial test suite**.

Reasons:

- CI environments often do not provide `mpirun`.
- Most correctness bugs are algorithmic and show up in serial.
- MPI introduces *environmental* failure modes (launcher, hostfile, fabric, etc.) that are not useful noise during early development.

### But we still need to know MPI works

We include a small **MPI smoke test** that is **excluded by default**:

- File: `tests/test_mpi_smoke.cpp`
- CTest label: `mpi`
- Run it explicitly:

```bash
RUN_MPI_TESTS=1 ./build_and_run.sh
```

What it checks:

- MPI runtime launches multiple ranks.
- Collectives work (`MPI_Allreduce`).

What it does *not* check:

- Domain correctness (GW/DNA results)
- Performance or scaling

That separation is intentional.

---

## 6) Where you add MPI/OpenMP in Stage 2

Stage 2 will ask you to fill in parallel behavior at marked locations.

Common markers you’ll see in code:

- **[MPI-LIFECYCLE]** — initialize/finalize and rank/size
- **[MPI-DISTRIBUTED-MODE]** — when to use MPI distribution
- **[MPI-WORK-DISTRIBUTION]** — scatter/gather patterns
- **[OMP-THREADS]** — controlling and using OpenMP threads

When you implement a marker, add a short comment like:

```cpp
// Implemented Stage 2: see PARALLEL_TUTORIAL.md §4
```

This keeps the code and tutorial cross-referenced.

---

## Appendix: CLI examples

Serial (single process):

```bash
./build/lacc_exec --domain gw --outdir out --distributed off
```

Distributed (two ranks on one machine):

```bash
mpirun -n 2 ./build/lacc_exec --domain gw --outdir out --distributed auto
```

Hybrid (two ranks, four threads each):

```bash
mpirun -n 2 ./build/lacc_exec --domain gw --outdir out --threads 4 --distributed auto
```
