#!/bin/bash
rm nbody
rm *.o
g++ -c -std=c++11 -Ofast -fopenmp nbody_hp.cpp
g++ -o nbody nbody_hp.o

n=( 10 20 50 100 200 500 1000 2000 5000 10000 )
p=( 1, 4, 8, 16, 18, 20 )
#n=( 5 10 20 50 )

echo "Form: N, P, Time(Microseconds)" >> hp.csv

for i in "${n[@]}"
do
    echo "Running size $i" 
    for j in "${p[@]}"
    do
        echo "Running thread $j" 
        ./nbody "$i $j" >> hp.csv
    done

done
