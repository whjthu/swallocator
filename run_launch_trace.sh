# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/$1.0.log ./test_trace.out trace memtest_trace/$1.memlog 0
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/$1.1.log ./test_trace.out trace memtest_trace/$1.memlog 1
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/$1.2.log ./test_trace.out trace memtest_trace/$1.memlog 2
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/$1.3.log ./test_trace.out trace memtest_trace/$1.memlog 3
# bsub -b -akernel -q q_test_ss -n 1 -cgsp 64 -host_stack 1024 -share_size 4000 -ro_size 256 -cache_size 0 -o memtest_log/$1.4.log ./test_trace.out trace memtest_trace/$1.memlog 4

./test_trace.out trace memtest_trace/$1.memlog 0 > memtest_log/$1.0.log
./test_trace.out trace memtest_trace/$1.memlog 1 > memtest_log/$1.1.log
./test_trace.out trace memtest_trace/$1.memlog 2 > memtest_log/$1.2.log
./test_trace.out trace memtest_trace/$1.memlog 3 > memtest_log/$1.3.log
./test_trace.out trace memtest_trace/$1.memlog 4 > memtest_log/$1.4.log
