Instructor-only notes.


## MPI testing note

By default, the project runs a **serial** CTest suite (no `mpirun`). This is deliberate: it keeps the correctness tests focused on ADTs/algorithms and makes local development straightforward.

There is an optional **MPI smoke test** (`test_mpi_smoke`) that validates the toolchain and runtime wiring. It is labeled `mpi` and excluded by default; enable it by running:

```bash
RUN_MPI_TESTS=1 ./build_and_run.sh
```

For cluster runs, you can also invoke the main program directly under MPI, for example:

```bash
mpirun -n 4 ./build/lacc_exec --domain gw --outdir build/out --distributed auto
```

