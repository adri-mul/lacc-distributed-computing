# LACC Assignments (Common Rules)

All teams build **one** C++ command-line tool named `lacc_analyze` that runs on a single machine (no SLURM/OpenMP yet).

## Required CLI (identical across all teams)

```bash
lacc_analyze --input <path> --mode <MODE> --k <K> --topk <N> --threshold <T> --outdir <DIR>
```

Notes:
- Physics teams ignore `--k` (but must accept it).
- Biology teams ignore `--threshold` (but must accept it).
- `--mode` differs per team (see your specific assignment).

## Required output files (identical across all teams)

Your program must write these files to `--outdir`:

### `summary.json` (required keys)
- `team_id`
- `mode`
- `input_file`
- `n_records_processed`
- `runtime_ms`
- `topk`
- `threshold`
- `k`
- `result_primary_value` (meaning depends on track)
- `result_primary_time_or_index` (meaning depends on track)
- `notes` (short string)

### `candidates.csv`
Header must be exactly:

```csv
rank,score,time_or_index,label
```

Rules:
- `rank` starts at 1
- `score` = SNR (physics) or count/enrichment score (biology)
- `time_or_index` = GPS time (physics) or read index/position (biology; see track)
- `label` = short string you generate (e.g., `peak`, `glitch_like`, `high_kmer`, `enriched`)

### `histogram.csv`
Header must be exactly:

```csv
bin,value
```

Rules:
- `bin` = integer bin id (0..B-1)
- `value` = count in that bin
- binning strategy is up to you, but must be deterministic

### `run.log`
A plain-text log showing:
- parsed arguments
- any warnings
- summary of steps performed

## Grading hooks (automatable, identical)
CI will run `./lacc_analyze` on a small hidden dataset and check:
- program exits 0
- files exist
- headers match exactly
- JSON parses
- candidate list sorted descending by score
- histogram bins are contiguous
