# Benchmarking Alternative Implementations

This folder contains scripts and configurations for running alternative implementations for comparison with our method. The job scripts are optimized for **SLURM-based HPC environments**.

## 📂 Folder Structure
- `Parasail/` - SLURM job script for running **Parasail** on HPC.
- `SeqAN/` - SLURM job script and source code for running **SeqAN** on HPC.
- `SWIPE/` - SLURM job script for running **SWIPE** on HPC.


## 🚀 Running the Benchmarks on an HPC Cluster

The provided **SLURM job scripts** allow efficient benchmarking execution. Follow the steps below to submit and monitor jobs.

---

### 1️⃣ Parasail
**Description**:
- **Parasail** is a **high-performance sequence alignment library** optimized with **SIMD instructions (SSE, AVX, etc.)**.
- Implements **Smith-Waterman (SW), Needleman-Wunsch (NW), and semi-global alignment**.
- Used for **fast sequence similarity searches**.

**Submit Job to SLURM**:
```sh
sbatch benchmarks/Parasail/Parasail.sh
```

**Command Used**:
```sh
srun parasail_aligner -a sw_trace_scan_sse41_128_sat -d -e 2 -o 0 -M 2 -X 1 -t 40 -v -V -f BRCA1.fasta -g BRCA1Ou2.cvs -O SAM
```

**Explanation of Parameters**:
- `-a sw_trace_diag_sse41_128_sat` → **Smith-Waterman alignment** with **SSE4.1** optimizations.
- `-d` → **Assume DNA alphabet ACGT**.
- `-e 2 -o 0` → **Gap penalties** (extension = `-e`, opening = `-o`).
- `-M 2` → **Match score**.
- `-X 1` → **Mismatch score**.
- `-t 40` → Use **40 CPU threads**.
- `-V -v` → **Verbose output**.
- `-f BRCA1.fasta` → **Input sequence file**.
- `-g BRCA1.csv` → **Output file** (CSV format).
- `-O SAM` → Output alignment in **SAM format**.

---

### 2️⃣ SeqAN
**Description**:
- **SeqAN** is a **C++ library for sequence analysis**, providing efficient **alignment algorithms** for **high-performance computing**.
- Supports **Smith-Waterman, Needleman-Wunsch, and semi-global alignment**.
- Uses **modern C++ optimizations** for high efficiency.

**Submit Job to SLURM**:
```sh
cd benchmarks/SeqAN
git clone https://github.com/seqan/seqan3.git
cd build 
sbatch SeqAN.sh
```

**Command Used**:
```sh
cmake -DCMAKE_BUILD_TYPE=Release ../source
make
./SeqAN
```

**Explanation of Parameters**:
- `cmake -DCMAKE_BUILD_TYPE=Release ../source` → **Build configuration step**.
- `make` → **Compile SeqAN**.
- `./SeqAN` → Run SeqAN


---


### 3️⃣ SWIPE
**Description**:
- **SWIPE (Smith-Waterman Implementation using Parallel Execution)** is a **high-speed Smith-Waterman algorithm** optimized for **multi-threading** and **vectorized execution**.
- Designed for **fast large-scale sequence alignments**.

**Submit Job to SLURM**:
```sh
cd benchmarks/SWIPE/
git clone https://github.com/torognes/swipe.git
cd swipe
mv * .??* ..
cd ..
rm -rf swipe
make
sbatch SWIPE.sh
```

**Command Used**:
```sh
./swipe -p 0 -S 1 -i BRCA1.fasta -d BRCA1_db -r 2 -q -1 -G 0 -E 2 -a 40 -o swipeoutputBRCA1-40.txt
```

**Explanation of Parameters**:
- `-p 0` → **symbol type/translation [0-4]**.
- `-S 1` → **query strands to search [1-3]**.
- `-i BRCA1.fasta` → **query sequence filename (stdin)**.
- `-d BRCA1_db` → **sequence database base name**.
- `-r 2` → **reward for nucleotide match**.
- `-q -1` → **penalty for nucleotide mismatch**.
- `-G 0` → **gap open penalty**.
- `-E 2` → **gap extension penalty**.
- `-a 40` → **number of threads to use [1-256]**.
- `-o swipeoutputBRCA1-40.txt` → **output file (stdout)**.

---

## 🗒️ Note

- Ensure all dependencies (`Parasail`, `SWIPE`, `SeqAN`, etc.) are **installed and loaded** before running.

---

