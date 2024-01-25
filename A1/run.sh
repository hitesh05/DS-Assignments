#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <filename> <num_processes>"
    exit 1
fi

filename="$1"
num_processes="$2"

mpicxx "$filename"
mpirun -np "$num_processes" ./a.out