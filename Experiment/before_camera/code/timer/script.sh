#!/bin/bash
set -xe
if [ $# -ne 1 ] 
then
  echo "Must provide a benchmark name"
else 
  echo "Check benchmark $1"
  #if [ ! -f "$1" ]; then
  #  echo "Invalid benchmark name $1"
  #  exit 1
  #fi  

  cpu_num=( 0 1 2 3 4 5 6 7 8 9 10 11 12 13 )
  #cpu_num=( 13 )

  for i in "${cpu_num[@]}"
  do  
    CORES=$(($i+1))
    FILENAME="result_$1_$CORES"
    echo "Running $1 using $CORES cores"  
    taskset -c 0-$i ./ot $CORES | tee -a hpec_result_tbb
  done
fi  
