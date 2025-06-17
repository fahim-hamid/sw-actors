# Benchmarking SW-Actors and Alternative Implementations

This folder contains scripts and configurations for running **SW-Actors** benchmarks and comparing performance with alternative sequence alignment implementations. The job scripts are optimized for **SLURM-based HPC environments**.

## 📂 Folder Structure
- `Actor/` - SLURM job scripts for **SW-Actors** single-node benchmarks
- `Actor-Multinode/` - SLURM job scripts for **SW-Actors** distributed (multi-node) benchmarks
- `Parasail/` - SLURM job script for running **Parasail** on HPC
- `SeqAN/` - SLURM job script and source code for running **SeqAN** on HPC
- `SWIPE/` - SLURM job script for running **SWIPE** on HPC


## 🎯 SW-Actors Benchmarks

### Single-Node Benchmarks (`Actor/`)
These scripts test SW-Actors performance on a single node with varying numbers of CPU cores and different datasets:

**Available Job Scripts:**
- **BRCA1**: `BRCA1-{1,5,10,15,20,25,30,35,40}.sh` - BRCA1.fasta with 1-40 CPU cores
- **BRCA2**: `BRCA2-{1,5,10,15,20,25,30,35,40}.sh` - BRCA2.fasta with 1-40 CPU cores  
- **Titin**: `Titin-40.sh` -  with 40 CPU cores
- **Heterogeneous**: `Heterogeneous-40.sh` - with 40 CPU cores)

**Example Usage:**
```bash
# Run BRCA1 benchmark with 40 CPU cores
sbatch benchmarks/Actor/BRCA1-40.sh

# Run BRCA2 benchmark with 20 CPU cores  
sbatch benchmarks/Actor/BRCA2-20.sh

```

### Multi-Node Distributed Benchmarks (`Actor-Multinode/`)
These scripts test SW-Actors distributed computing capabilities across multiple nodes:

**Available Configurations:**
- **2-Node**: `BRCA1-2-40.sh`, `BRCA1-2-80.sh`, `BRCA2-2-40.sh`, `BRCA2-2-80.sh`
- **4-Node**: `BRCA1-4-40.sh`, `BRCA1-4-80.sh`, `BRCA1-4-160.sh`, `BRCA2-4-40.sh`, `BRCA2-4-80.sh`, `BRCA2-4-160.sh`

**Example Usage:**
```bash
# Run BRCA1 on 2 nodes with 40 total CPU cores
sbatch benchmarks/Actor-Multinode/BRCA1-2-40.sh

# Run BRCA1 on 4 nodes with 160 total CPU cores
sbatch benchmarks/Actor-Multinode/BRCA1-4-160.sh
```

**Sample Multi-Node Execution:**
```bash
# Server node (first node)
./swActor -D -s -p 4424 -Q ../dataset/BRCA1.fasta -O BRCA1-2-40.SAM -A 20 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=20

# Client node (additional nodes)
./swActor -D -p 4424 -H <server_ip> -A 20 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=20
```

---

## 🚀 Running Comparison Benchmarks

The following sections describe how to run alternative implementations for performance comparison against SW-Actors.

---

### 1️⃣ Parasail
**Description**:
- **Parasail** is a **high-performance sequence alignment library** optimized with **SIMD instructions (SSE, AVX, etc.)**.
- Implements **Smith-Waterman (SW), Needleman-Wunsch (NW), and semi-global alignment**.
- Used for **fast sequence similarity searches**.

**Submit Job to SLURM**:
```sh
sbatch benchmarks/Parasail/Parasail-BRCA1.sh
# or
sbatch benchmarks/Parasail/Parasail-Titin.sh
```

**Command Used**:
```sh
srun parasail_aligner -a sw_trace_diag_16 -d -e 2 -o 0 -M 2 -X 1 -t 40 -v -V -f BRCA1.fasta -g BRCA1.cvs -O SAM
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

- Ensure all dependencies (`Parasail`, `SWIPE`, `SeqAN`, etc.) are **installed and loaded** before running comparison benchmarks.
- SW-Actors benchmarks require the C++ Actor Framework to be properly installed and configured.
- All benchmark scripts are designed for SLURM-based HPC environments and may need modification for other job schedulers.
- Multi-node SW-Actors benchmarks require proper network configuration between compute nodes.

## Credits
The benchmarking setup presented here relies on the work and tools developed by the authors of **Parasail**, **SeqAN**, and **SWIPE**, whose implementations form the basis of our performance comparisons. We gratefully acknowledge their contributions to the bioinformatics community.

**SW-Actors** implements the Smith-Waterman algorithm using the Actor Model for parallel and distributed computing, providing a novel approach to sequence alignment that can be compared against these established tools.

- **Parasail:** Daily, J., 2016. Parasail: SIMD C library for global, semi-global, and local pairwise sequence alignments. *BMC Bioinformatics*, [doi:10.1186/s12859-016-0930-z](https://doi.org/10.1186/s12859-016-0930-z)
- **SeqAN:** Reinert, K., et al., 2017. The SeqAn C++ template library for efficient sequence analysis: A resource for programmers. *Journal of Biotechnology*, [doi:10.1016/j.jbiotec.2017.07.017](https://doi.org/10.1016/j.jbiotec.2017.07.017)
- **SWIPE:** Rognes, T., 2011. Faster Smith–Waterman database searches with inter-sequence SIMD parallelisation. *BMC Bioinformatics*, [doi:10.1186/1471-2105-12-221](https://doi.org/10.1186/1471-2105-12-221)

---

