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

# Inser heavy
${program} -n 100000 --inserts 0.8 --removals 0.1 --lookups 0.1 --preload 0.2 --type cuckoo,tbb
${program} -n 100000 --inserts 0.5 --removals 0.25 --lookups 0.25 --preload 0.5 --type cuckoo,tbb

# Remove heavy
${program} -n 100000 --inserts 0.1 --removals 0.8 --lookups 0.1 --preload 0.9 --type cuckoo,tbb
${program} -n 100000 --inserts 0.25 --removals 0.5 --lookups 0.25 --preload 0.75 --type cuckoo,tbb

# Lookup heavy
${program} -n 100000 --inserts 0.1 --removals 0.1 --lookups 0.8 --preload 0.9 --type cuckoo,tbb
${program} -n 100000 --inserts 0.25 --removals 0.25 --lookups 0.5 --preload 0.75 --type cuckoo,tbb

date_now=$(date)
echo
echo "##### Job completed at: ${date_now} #####"

