#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=185G
#SBATCH --time=12:00:00
#SBATCH --output=Parasail-BRCA1-40cpu-%j.out
#SBATCH --constraint=skylake

# Navigate to the submission directory
cd $SLURM_SUBMIT_DIR

echo "Parasail BRCA1 40"

time srun ../apps/parasail_aligner -a sw_trace_diag_16 -d -e 2 -o 0 -M 2 -X 1 -t 40 -v -V -f ../../dataset/BRCA1.fasta -g BRCA1-40.cvs -O SAM

