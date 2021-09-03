for i in bgl_3072 bgl_6144 blackscholes bodytrack canneal dedup facesim ferret fluidanimate freqmine raytrace streamcluster swaptions vips x264
do
    echo $i
    ./run_launch_trace.sh $i
done
