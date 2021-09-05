# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/rand_$1_$2.0.log ./test_trace.out rand memtest_trace/rand_$1_$2.conf 0
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/rand_$1_$2.1.log ./test_trace.out rand memtest_trace/rand_$1_$2.conf 1
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/rand_$1_$2.2.log ./test_trace.out rand memtest_trace/rand_$1_$2.conf 2
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/rand_$1_$2.3.log ./test_trace.out rand memtest_trace/rand_$1_$2.conf 3
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/rand_$1_$2.4.log ./test_trace.out rand memtest_trace/rand_$1_$2.conf 4

./test_trace.out rand memtest_trace/rand_$1_$2.conf 0 > memtest_log/rand_$1_$2.0.log
./test_trace.out rand memtest_trace/rand_$1_$2.conf 3 > memtest_log/rand_$1_$2.3.log
./test_trace.out rand memtest_trace/rand_$1_$2.conf 4 > memtest_log/rand_$1_$2.4.log
