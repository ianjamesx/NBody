#!/bin/bash
rm nbody
rm *.o
g++ -c -std=c++11 nbody_ap.cpp
g++ -o nbody nbody_ap.o

n=( 10 20 50 100 200 500 1000 2000 5000 10000 )
#n=( 5 10 20 50 )

echo "Form: N, Time(Microseconds)" >> ap.txt
for i in "${n[@]}"
do
    echo "Running size $i" 
    if [ $i -lt 2001 ]
    then
        for j in {1..5}
        do
            ./nbody "$i" >> ap.txt
        done
    else
       ./nbody "$i" >> ap.txt
    fi
done
