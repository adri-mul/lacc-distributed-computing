# Team P2 — Artifact / Glitch Characterization (Physics)

**Mode:** `--mode artifact`

## Scientific context
You are given a LIGO-like strain time series that contains **no injected chirp**, but does include noise bursts / glitch-like artifacts.

## Objective
Run the same matched-filter pipeline and characterize false triggers from noisy/glitchy data.

## Input format
CSV with header:

```csv
time,strain
```

## Required algorithm (proof-of-concept)
Same as P1:
1. Generate a linear chirp template
2. Sliding normalized dot product → “SNR-like” time series
3. Identify top-K local maxima above `--threshold`

## Output interpretation
- `result_primary_value` = best (max) SNR
- `result_primary_time_or_index` = time of best SNR peak

## Additional requirements beyond the common rules
In `summary.json`, include an additional field:
- `false_trigger_rate_est` (triggers per second above threshold)

In `candidates.csv`, set `label` using a simple rule you define:
- `glitch_like` if the peak is very narrow (e.g., falls off quickly)
- `broad` otherwise

You must document your rule in README; keep it simple.

## Suggested analysis paragraph (in your repo README)
Explain:
- why real data produces false triggers
- one mitigation idea (even if not implemented yet)
