#!/bin/bash
#SBATCH --account=hpc_p_spiteri
#SBATCH --nodes=1
#SBATCH --cpus-per-task=5
#SBATCH --mem=185GB
#SBATCH --time=12:00:00
#SBATCH --output=AS-BRCA1-5cpu-%j.out
#SBATCH --constraint=skylake


cd $SLURM_SUBMIT_DIR/../..


export LD_LIBRARY_PATH=$PWD/dependencies/install/caf/lib64:$LD_LIBRARY_PATH


make clean
make
cd build


# Run your program
echo "BRCA1 1 node 5 cpu"

time ./swActor -Q ../dataset/BRCA1.fasta -O BRCA1-5.SAM -A 5 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=5
time ./swActor -Q ../dataset/BRCA1.fasta -O BRCA1-5.SAM -A 5 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=5
time ./swActor -Q ../dataset/BRCA1.fasta -O BRCA1-5.SAM -A 5 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=5
time ./swActor -Q ../dataset/BRCA1.fasta -O BRCA1-5.SAM -A 5 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=5
time ./swActor -Q ../dataset/BRCA1.fasta -O BRCA1-5.SAM -A 5 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=5