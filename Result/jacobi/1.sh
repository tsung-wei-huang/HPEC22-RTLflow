cpu_num=( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 )
for i in "${cpu_num[@]}"
do  
  CORES=$(($i+0))
  FILENAME="result_jacobi_stencil_$CORES"
  awk 'NR==162' $FILENAME >> scalability
done
