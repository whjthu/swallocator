CC = g++

all: test_size.out test_trace.out


blas:
	$(MAKE) -C .. ar 

# CFLAGS = -std=c++17 -O0 -msimd -I../include/ -mieee -fPIC
CFLAGS = -std=c++17 -O3 -fPIC -g #-D_DEBUG
# CFLAGS += -DDEBUG
# CFLAGS += -mfuse -D_ATHREAD -fPIC -DDEBUG
CFLAGS += -fPIC
# CFLAGS += -DDEBUG_SLAVE_VAL

# LIBFLAGS = -mfuse -lakernel
# LIBFLAGS = -mdynamic
# LIBFLAGS = -mhybrid

SRC_TEST = CAllocator.cpp FreeListAllocator.cpp BlockAllocator.cpp test_size.cpp
SRC_TRACE = CAllocator.cpp FreeListAllocator.cpp BlockAllocator.cpp test_trace.cpp memops.cpp

test_size.out: $(SRC_TEST)
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ $(SRC_TEST) -o $@
test_trace.out: $(SRC_TRACE)
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ $(SRC_TRACE) -o $@


.PHONY: clean

clean:
	rm *.out
