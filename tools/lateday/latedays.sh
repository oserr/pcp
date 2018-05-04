#!/bin/bash
# Submits one job per script.

for op in insert lookup removal
do
    for load in heavy medium
    do
        qsub ${op}_${load}.sh
    done
done
