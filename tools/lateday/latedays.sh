#!/bin/bash
# Submits one job per script.

for op in insert lookup removal
do
    for load in heavy medium
    do
        qsub -q timer ${op}_${load}.sh
    done
done
