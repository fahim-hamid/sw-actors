#!/bin/bash
#SBATCH --nodes=4
#SBATCH --cpus-per-task=40
#SBATCH --mem=185G
#SBATCH --time=12:00:00
#SBATCH --output=MS-BRCA1-4-160cpu%j.out
#SBATCH --constraint=skylake

echo "4 node 160 cpu"

counter=0
IP=$(srun --nodes=1 --ntasks=1 hostname -i)

cd $SLURM_SUBMIT_DIR
  
cmake --preset release
cmake --build --preset release

for node in $(scontrol show hostnames); do
    echo $node

    if [ $counter -eq 0 ]; then
        IP=$(srun --nodes=1 --ntasks=1 hostname -i)
        srun --nodes=1 --ntasks=1 ./build/release/swActor -D -s -p 44416 -Q ./dataset/BRCA1.fasta -O BRCA1-4-160.SAM -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40&
        sleep 2
        counter=$((counter + 1))
        echo "lets do"
    else
        echo "lets do others"
        srun --nodes=1 --ntasks=1 ./build/release/swActor -D -p 44416 -H $IP -A 40 -m 2 -M -1 -g -2 -R 1 -C 1 --caf.scheduler.max-threads=40&
    fi
done

wait
echo " 4 node 160 core done!"
