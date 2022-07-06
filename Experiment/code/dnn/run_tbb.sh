taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 10 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 20 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 30 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 40 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 50 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 60 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 70 -m tbb  
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 80 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 90 -m tbb  
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 100 -m tbb 

#for e in {10,30,50,70,90}; do
#  echo "taskset -c 0-7 ./parallel_dnn -t 8 -r 10 -e $e";
##   eval "taskset -c 0-7 ./parallel_dnn -t 8 -r 10 -e $e | tee -a result_tf";
##  echo $i;
##  j=$(($i-1))
#done

