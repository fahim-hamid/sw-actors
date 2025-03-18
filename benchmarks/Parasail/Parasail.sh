#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=185G
#SBATCH --time=12:00:00
#SBATCH --output=Parasail-BRCA1-40cpu-%j.out
#SBATCH --constraint=skylake

cd $SLURM_SUBMIT_DIR

module load StdEnv/2023 parasail/2.6.2

echo "Parasail BRCA1 40"

time srun parasail_aligner -a sw_trace_scan_sse41_128_sat -d -e 2 -o 0 -M 2 -X 1 -t 40 -v -V -f BRCA1.fasta -g BRCA1Ou2.cvs -O SAM

