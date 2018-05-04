#!/bin/bash
# Limit execution time to 20 minutes
#PBS -lwalltime=0:40:00
# Allocate all available CPUs on a single node
#PBS -l nodes=1:ppn=24

# Go to the directory from which you submitted your job
cd $PBS_O_WORKDIR

export LD_LIBRARY_PATH=/opt/gcc/4.9.2/lib64:/opt/gcc/4.9.2/lib:${LD_LIBRARY_PATH}

# Insert Heavy
./build/src/benchmark -n 1000000 --inserts 0.8 --removals 0.1 --lookups 0.1 --preload 0.2
./build/src/benchmark -n 1000000 --inserts 0.5 --removals 0.25 --lookups 0.25 --preload 0.5

# Lookup heavy
./build/src/benchmark -n 1000000 --inserts 0.1 --removals 0.1 --lookups 0.8 --preload 0.9
./build/src/benchmark -n 1000000 --inserts 0.25 --removals 0.25 --lookups 0.5 --preload 0.75

# Removal heavy
./build/src/benchmark -n 1000000 --inserts 0.1 --removals 0.8 --lookups 0.1 --preload 0.9
./build/src/benchmark -n 1000000 --inserts 0.25 --removals 0.5 --lookups 0.25 --preload 0.75
