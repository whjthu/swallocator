for i in 0 
do
    for j in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576
    do
        ./memtest_run.sh $j $i
    done
done
