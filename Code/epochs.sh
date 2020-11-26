#!/bin/bash

echo "file:  test_10000_10000_10.txt" &> epochs.txt
echo "Epochs: 10" &>> epochs.txt
echo "alpha = mu = 0.001" &>> epochs.txt

echo "Parallel" &>> epochs.txt
g++ -std=c++11  -fopenmp -o parallel parallel.cpp
export OMP_NUM_THREADS=10
(./parallel  test_10000_10000_10.txt 10) &>> epochs.txt

echo "AIG" &>> epochs.txt
g++ -std=c++11  -fopenmp -o aig aig.cpp
export OMP_NUM_THREADS=10
(./aig  test_10000_10000_10.txt 10) &>> epochs.txt

echo "HOGWILD!" &>> epochs.txt
g++ -std=c++11  -fopenmp -o hogwild hogwild.cpp
export OMP_NUM_THREADS=10
(./hogwild  test_10000_10000_10.txt 10) &>> epochs.txt

echo "Sequential" &>> epochs.txt
g++ -std=c++11  -fopenmp -o sequential sequential.cpp
(./sequential  test_10000_10000_10.txt 10) &>> epochs.txt
