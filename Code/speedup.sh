#!/bin/bash

echo "file:  test_10000_10000_10.txt" &> speedup.txt
echo "Epochs: 5" &>> speedup.txt
echo "alpha = mu = 0.001" &>> speedup.txt

echo "Sequential" &>> speedup.txt
g++ -std=c++11  -fopenmp -o sequential sequential.cpp
(time ./sequential  test_10000_10000_10.txt 1) &> speedup.txt

echo "Parallel" &>> speedup.txt
g++ -std=c++11  -fopenmp -o parallel parallel.cpp
export OMP_NUM_THREADS=1
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=2
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=3
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=4
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=5
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=6
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=7
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=8
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=9
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=10
(time ./parallel  test_10000_10000_10.txt 1) &>> speedup.txt

echo "AIG" &>> speedup.txt
g++ -std=c++11  -fopenmp -o aig aig.cpp
export OMP_NUM_THREADS=1
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=2
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=3
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=4
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=5
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=6
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=7
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=8
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=9
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=10
(time ./aig  test_10000_10000_10.txt 1) &>> speedup.txt

echo "HOGWILD!" &>> speedup.txt
g++ -std=c++11  -fopenmp -o hogwild hogwild.cpp
export OMP_NUM_THREADS=1
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=2
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=3
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=4
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=5
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=6
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=7
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=8
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=9
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
export OMP_NUM_THREADS=10
(time ./hogwild  test_10000_10000_10.txt 1) &>> speedup.txt
