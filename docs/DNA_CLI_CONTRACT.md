
# 🧬 DNA CLI Contract  
## LACC Environmental Sequencing Analysis

---

## 1. Purpose

This document defines the **authoritative command-line interface (CLI) contract** for all DNA analysis programs in the LACC project.

All DNA implementations — regardless of scientific objective — **must expose identical CLI behavior**, flags, defaults, and error handling.

This contract applies to:
- **SoW–DNA-A:** Environmental Abundance Profiling  
- **SoW–DNA-B:** Differential Enrichment Analysis  

No Statement of Work may override or contradict this contract.

---

## 2. Executable Name

All DNA analysis programs must produce an executable named:

	lacc_dna


The executable must be invokable from the command line and compatible with MPI execution.

Example:
```bash
mpirun -np 4 ./lacc_dna [options]