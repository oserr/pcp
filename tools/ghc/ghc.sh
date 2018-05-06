#!/bin/bash
# Run all tests that were run on latedays

set -o xtrace

program=../../build/src/benchmark
${program} -n 100000 -i 0.8 -r 0.1 -l 0.1 -p 0.2 > combined_insert_heavy.txt
${program} -n 100000 -i 0.5 -r 0.25 -l 0.25 -p 0.5 > combined_insert_medium.txt
${program} -n 100000 -i 0.1 -r 0.1 -l 0.8 -p 0.9 > combined_lookup_heavy.txt
${program} -n 100000 -i 0.25 -r 0.25 -l 0.5 -p 0.75 > combined_lookup_medium.txt
${program} -n 100000 -i 0.1 -r 0.8 -l 0.1 -p 0.9 > combined_removal_heavy.txt
${program} -n 100000 -i 0.25 -r 0.5 -l 0.25 -p 0.75 > combined_removal_medium.txt

${program} -n 10000000 -i 0.8 -r 0.1 -l 0.1 -p 0.2 --map-only > maponly_insert_heavy.txt
${program} -n 10000000 -i 0.1 -r 0.1 -l 0.8 -p 0.9 --map-only > maponly_lookup_heavy.txt
${program} -n 10000000 -i 0.1 -r 0.8 -l 0.1 -p 0.9 --map-only > maponly_removal_heavy.txt
${program} -n 10000000 -l 1 -p 1 --map-only > maponly_lookup_only.txt
