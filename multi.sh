#!/bin/bash
#SBATCH --nodes=4
#SBATCH --cpus-per-task=40
#SBATCH --mem=180G
#SBATCH --time=12:00:00
#SBATCH --output=MS-BRCA1-4-160cpu%j.out
#SBATCH --constraint=skylake

echo "4 node 160 cpu"

counter=0
IP=$(srun --nodes=1 --ntasks=1 hostname -i)
# Run the code
for node in $(scontrol show hostnames); do
    echo $node
export LD_LIBRARY_PATH := /path/to/your/library:$LD_LIBRARY_PATH

    cd $SLURM_SUBMIT_DIR
    
    make clean
    make
    cd build

    if [ $counter -eq 0 ]; then
        IP=$(srun --nodes=1 --ntasks=1 hostname -i)
        srun --nodes=1 --ntasks=1 ./swActor -D -s -p 4444 -Q dataset/BRCA1.fasta -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40&
        sleep 2
        counter=$((counter + 1))
        echo "lets do"
    else
        echo "lets do others"
        srun --nodes=1 --ntasks=1 ./swActor -D -p 4444 -H $IP -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40&
    fi
done

wait
echo " 4 node 160 core done!"
