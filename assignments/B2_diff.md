# Team B2 — Differential / Enriched k-mers (Biology)

**Mode:** `--mode diff`

## Scientific context
Comparing two samples can reveal k-mers enriched in one sample versus another.

## Objective
Compare two samples and report k-mers enriched in sample B versus sample A.

## Input convention
`--input` points to a directory containing:
- `sampleA.txt`
- `sampleB.txt`

Each file uses the same simplified FASTA-like format as B1.

## Required algorithm (proof-of-concept)
1. Extract k-mers (length `--k`) from each sample
2. Count k-mers in chained hash tables
3. Compute an enrichment score per k-mer (document your choice), e.g.:
   - `score = countB - countA`
   - (or ratio with pseudocount)

## Output interpretation
- `result_primary_value` = count (or enrichment score) of the top candidate
- `result_primary_time_or_index` = integer read index where the top candidate was first observed (0-based)

## Additional requirements beyond the common rules
In `summary.json`, include:
- `n_enriched_kmers` (score > 0)
- `top_enrichment_score`

In `candidates.csv`, set `label`:
- `enriched` if score > 0
- `depleted` otherwise (if you include negatives)

## Suggested analysis paragraph (in your repo README)
Explain:
- why sequencing errors inflate singletons
- what enrichment means conceptually
