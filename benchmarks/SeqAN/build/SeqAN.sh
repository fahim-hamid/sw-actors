#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=185G
#SBATCH --time=12:00:00
#SBATCH --output=SeqAN-BRCA1-40cpu-%j.out
#SBATCH --constraint=skylake

# Load necessary modules
module load gcc/11.3.0 cmake/3.27.7

cd $SLURM_SUBMIT_DIR

cmake -DCMAKE_BUILD_TYPE=Release ../source

make

echo "SeqAN BRCA1 40"

time ./SeqAN



