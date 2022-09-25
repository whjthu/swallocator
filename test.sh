#!/bin/bash
for i in {0..5}
do
    ./test_trace.out rand config.txt $i
done
