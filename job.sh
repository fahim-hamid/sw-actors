#!/bin/bash
#SBATCH --account=hpc_p_spiteri
#SBATCH --nodes=1
#SBATCH --cpus-per-task=40
#SBATCH --mem=185GB
#SBATCH --time=01:00:00
#SBATCH --constraint=skylake


# Navigate to the directory containing your MPI program
cd $SLURM_SUBMIT_DIR

# Set LD_LIBRARY_PATH if needed
export LD_LIBRARY_PATH=/globalhome/pma753/HPC/root/install/lib64:$LD_LIBRARY_PATH

make clean
make

echo "40 cpus 40 actors"
echo "BRCA1.fasta"

# Run your program
time ./swActor --caf.scheduler.max-threads=40 > outputjob.txt
 