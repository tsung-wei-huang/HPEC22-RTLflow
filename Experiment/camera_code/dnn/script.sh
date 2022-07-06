set -x
for i in {10,20,30,40,50,60,70,80,90,100}
do
  taskset -c 0-9 ./parallel_dnn -t 10 -m tbb -r 10 -e $i | tee -a result_tbb
done
