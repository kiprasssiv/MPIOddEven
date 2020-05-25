#!/bin/sh
#SBATCH -p short
#SBATCH -n3
#SBATCH -C alpha
mpic++ -o oddEven main.cpp
mpirun oddEven 10000
