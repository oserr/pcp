#!/bin/bash
# Limit execution time to 8 hours
#PBS -lwalltime=8:00:00
# Allocate all available CPUs on a single node
#PBS -l nodes=1:ppn=24

# Go to the directory from which you submitted your job
cd $PBS_O_WORKDIR

export LD_LIBRARY_PATH=/opt/gcc/4.9.2/lib64:/opt/gcc/4.9.2/lib:${LD_LIBRARY_PATH}

date_now=$(date)
echo "##### Job started at: ${date_now} #####"
echo

./build/src/benchmark -n 1000000 --inserts 0.25 --removals 0.5 --lookups 0.25 --preload 0.75

date_now=$(date)
echo
echo "##### Job completed at: ${date_now} #####"
