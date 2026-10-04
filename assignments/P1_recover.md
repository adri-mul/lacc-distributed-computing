# Team P1 — Known Event Recovery (Physics)

**Mode:** `--mode recover`

## Scientific context
You are given a LIGO-like strain time series dominated by noise. A simplified “chirp-like” signal has been injected.

## Objective
Demonstrate that your pipeline can recover the strongest candidate signal in a dataset that contains a known injected chirp-like event.

## Input format
CSV with header:

```csv
time,strain
```

- `time` is seconds (GPS-like float)
- `strain` is float

## Required algorithm (proof-of-concept)
Compute a matched-filter-like score:

1. Generate a linear chirp template (parameters provided or hardcoded defaults)
2. Sliding normalized dot product → produce an “SNR-like” time series
3. Identify top-K local maxima above `--threshold`

## Output interpretation
- `result_primary_value` = best (max) SNR
- `result_primary_time_or_index` = time of best SNR peak

## Additional requirements beyond the common rules
In `summary.json`:
- `notes` must include whether your best peak is within **±0.25 s** of the provided **reference time** (given in the assignment handout).

In `candidates.csv`, set `label`:
- `peak` for the top candidate
- `secondary` for others

## Suggested analysis paragraph (in your repo README)
Explain:
- why normalization matters
- what happens if you raise/lower threshold
