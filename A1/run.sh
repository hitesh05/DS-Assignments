#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <filename> <num_processes>"
    exit 1
fi

filename="$1"
num_processes="$2"

mpic++.openmpi "$filename"
time mpirun.openmpi -np "$num_processes" --use-hwthread-cpus --oversubscribe ./a.out < test