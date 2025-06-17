#!/bin/bash
#SBATCH --account=hpc_p_spiteri
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=1000GB
#SBATCH --time=3-00:00:00
#SBATCH --output=AS-Heterogeneous-40cpu-%j.out
#SBATCH --constraint=cooper


cd $SLURM_SUBMIT_DIR/../..


export LD_LIBRARY_PATH=$PWD/dependencies/install/caf/lib64:$LD_LIBRARY_PATH


make clean
make
cd build


# Run your program
echo "Heterogeneous 1 node 40 cpu"

time ./swActor -Q ../dataset/Heterogeneous.fasta -O Heterogeneous-40.SAM -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40