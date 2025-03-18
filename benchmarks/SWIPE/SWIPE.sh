#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=185G
#SBATCH --time=12:00:00
#SBATCH --output=SWIPE-BRCA1-40cpu-%j.out
#SBATCH --constraint=skylake

cd $SLURM_SUBMIT_DIR

module load StdEnv/2023 gcc/12.3 blast+/2.14.1

makeblastdb -in BRCA1.fasta -dbtype nucl -out BRCA1_db -title BRCA1 -parse_seqids -blastdb_version 4

echo "SWIPE BRCA1 40"

# Command to run SWIPE
time ./swipe -p 0 -S 1 -i BRCA1.fasta -d BRCA1_db -r 2 -q -1 -G 0 -E 2 -a 40 -o swipeoutputBRCA1-40.txt

