#!/bin/bash
# Limit execution time to 8 hours
#PBS -lwalltime=8:00:00
# Allocate all available CPUs on a single node
#PBS -l nodes=1:ppn=24

# Definitions
export LD_LIBRARY_PATH=/opt/gcc/4.9.2/lib64:/opt/gcc/4.9.2/lib:${LD_LIBRARY_PATH}
program=../../build/src/benchmark

# Go to the directory from which you submitted your job
cd $PBS_O_WORKDIR

date_now=$(date)
echo "##### Job started at: ${date_now} #####"
echo

${program} -n 10000000 --lookups 1 --preload 1 --map-only

date_now=$(date)
echo
echo "##### Job completed at: ${date_now} #####"
