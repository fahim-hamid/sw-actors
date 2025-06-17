#!/bin/bash
#SBATCH --account=hpc_p_spiteri
#SBATCH --nodes=2
#SBATCH --cpus-per-task=40
#SBATCH --mem=185G
#SBATCH --time=01:00:00
#SBATCH --output=MS-BRCA2-2-80cpu%j.out
#SBATCH --constraint=skylake

echo "2 node 80 cpu"

counter=0
IP=$(srun --nodes=1 --ntasks=1 hostname -i)

cd $SLURM_SUBMIT_DIR/../..
export LD_LIBRARY_PATH=$PWD/dependencies/install/caf/lib64:$LD_LIBRARY_PATH
   
make clean
make
cd build


# Run the code
for node in $(scontrol show hostnames); do
    echo $node
    if [ $counter -eq 0 ]; then
        IP=$(srun --nodes=1 --ntasks=1 hostname -i)
        srun --nodes=1 --ntasks=1 ./swActor -D -s -p 4428 -Q ../dataset/BRCA2.fasta -O BRCA2-2-40.SAM -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40&
        sleep 2
        counter=$((counter + 1))
    else
        srun --nodes=1 --ntasks=1 ./swActor -D -p 4428 -H $IP -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40&
    fi
done

wait
echo " 2 node 80 core done!"