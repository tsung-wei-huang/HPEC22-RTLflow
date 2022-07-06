taskset -c 0-1 ./parallel_dnn -t 2 -r 10 -e 20 -m tbb 
taskset -c 0-3 ./parallel_dnn -t 4 -r 10 -e 20 -m tbb 
taskset -c 0-5 ./parallel_dnn -t 6 -r 10 -e 20 -m tbb 
taskset -c 0-7 ./parallel_dnn -t 8 -r 10 -e 20 -m tbb 
taskset -c 0-9 ./parallel_dnn -t 10 -r 10 -e 20 -m tbb 
