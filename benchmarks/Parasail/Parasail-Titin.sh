#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=2000GB
#SBATCH --time=3-00:00:00
#SBATCH --output=Parasail-Titin-40-%j.out
#SBATCH --constraint=cooper

# Navigate to the submission directory
cd $SLURM_SUBMIT_DIR

echo "Parasail Titin 40"

time srun ../apps/parasail_aligner -a sw_trace_scan_32 -d -e 2 -o 0 -M 2 -X 1 -t 40 -v -V -f ../../dataset/Titin.fasta -g Titin-40.cvs -O SAM
