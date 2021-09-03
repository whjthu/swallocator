for i in 1M 2M 4M 8M
do
    for j in 256 512 1024 2048 4096 8192 16384
    do
        ./run_launch_rand.sh $i $j
    done
done
