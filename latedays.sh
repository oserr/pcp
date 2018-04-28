#!/bin/bash
# Limit execution time to 20 minutes
#PBS -lwalltime=0:20:00
# Allocate all available CPUs on a single node
#PBS -l nodes=1:ppn=24

# Go to the directory from which you submitted your job
cd $PBS_O_WORKDIR

export LD_LIBRARY_PATH=/opt/gcc/4.9.2/lib64:/opt/gcc/4.9.2/lib:${LD_LIBRARY_PATH}

# Execute the performance evaluation program and store summary in benchmark.out
# (XXXX will get replaced by randomly generated 4-digit sequence)
# The "-a o" option instructs it to use processor affinity directives that are compatible
# with the version of MPI running on the Latedays nodes
./build/src/lb
