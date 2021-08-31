# bsub -I -b -akernel -q q_debug_aiyy -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 ./test_mask_fill.out 2>&1 | tee mask_fill.log
# bsub -I -b -akernel -q q_sw_expr -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 ./test.out 2>&1 | tee mask_fill.log
MEMTEST_SIZE=$1 MEMTEST_SEED=$2 bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/memtest_hard_$1_$2.log ./test.out
