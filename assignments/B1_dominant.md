# Team B1 — Dominant k-mers (Biology)

**Mode:** `--mode dominant`

## Scientific context
DNA sequencers produce large sets of reads. A common analysis step is counting substrings of length **k** (“k-mers”).

## Objective
Identify the most frequent k-mers and summarize the frequency distribution.

## Input format (simplified FASTA-like)
- Lines starting with `>` are headers
- The next line is a DNA sequence: `ACGTN...`
- Ignore quality scores

Example:

```
>read_001
ACGTTACGTA
>read_002
TTTACGNNAA
```

## Required algorithm (proof-of-concept)
For each read:
1. Extract all k-mers (length `--k`) using a sliding window
2. Count k-mers in a **chained hash table**
3. Produce:
   - Top-K k-mers by count
   - Histogram of counts (deterministic binning)

## Output interpretation
- `result_primary_value` = count of the top candidate
- `result_primary_time_or_index` = integer read index where the top candidate was first observed (0-based)

## Additional requirements beyond the common rules
In `summary.json`, include:
- `n_unique_kmers`
- `singleton_fraction` (fraction of k-mers with count == 1)

In `candidates.csv`, set `label`:
- `high_kmer` for all rows

## Suggested analysis paragraph (in your repo README)
Explain the effect of changing `k` on:
- `n_unique_kmers`
- singleton fraction
