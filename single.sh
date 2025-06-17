#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=185GB
#SBATCH --time=12:00:00
#SBATCH --output=AS-BRCA1-40cpu-%j.out
#SBATCH --constraint=skylake


cd $SLURM_SUBMIT_DIR

export LD_LIBRARY_PATH=$PWD/dependencies/install/caf/lib64:$LD_LIBRARY_PATH


make clean
make
cd build


# Run your program
echo "BRCA1 1 node 40 cpu 40 actors"

time ./swActor -Q ../dataset/BRCA1.fasta -O BRCA1-40.SAM -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40