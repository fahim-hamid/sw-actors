#!/bin/bash
#SBATCH --account=hpc_p_spiteri
#SBATCH --nodes=1
#SBATCH --cpus-per-task=30
#SBATCH --mem=185GB
#SBATCH --time=12:00:00
#SBATCH --output=AS-BRCA2-30cpu-%j.out
#SBATCH --constraint=skylake


cd $SLURM_SUBMIT_DIR/../..


export LD_LIBRARY_PATH=$PWD/dependencies/install/caf/lib64:$LD_LIBRARY_PATH


make clean
make
cd build


# Run your program
echo "BRCA2 1 node 30 cpu"

time ./swActor -Q ../dataset/BRCA2.fasta -O BRCA2-30.SAM -A 30 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=30
time ./swActor -Q ../dataset/BRCA2.fasta -O BRCA2-30.SAM -A 30 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=30
time ./swActor -Q ../dataset/BRCA2.fasta -O BRCA2-30.SAM -A 30 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=30
time ./swActor -Q ../dataset/BRCA2.fasta -O BRCA2-30.SAM -A 30 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=30
time ./swActor -Q ../dataset/BRCA2.fasta -O BRCA2-30.SAM -A 30 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=30
