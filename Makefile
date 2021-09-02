CC = g++

all: test.out trace.out gen_trace.out


blas:
	$(MAKE) -C .. ar 

# CFLAGS = -std=c++17 -O0 -msimd -I../include/ -mieee -fPIC
CFLAGS = -std=c++17 -O0 -fPIC -g #-D_DEBUG
# CFLAGS += -DDEBUG
# CFLAGS += -mfuse -D_ATHREAD -fPIC -DDEBUG
CFLAGS += -fPIC
# CFLAGS += -DDEBUG_SLAVE_VAL

# LIBFLAGS = -mfuse -lakernel
# LIBFLAGS = -mdynamic
# LIBFLAGS = -mhybrid

SRC_TEST = CAllocator.cpp FreeListAllocator.cpp BlockAllocator.cpp test.cpp
SRC_TRACE = CAllocator.cpp FreeListAllocator.cpp BlockAllocator.cpp trace.cpp

# ./%: ./%.c
gen_trace.out: gen_trace.cpp
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ gen_trace.cpp -o $@
test.out: $(SRC_TEST)
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ $(SRC_TEST) -o $@
trace.out: $(SRC_TRACE)
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ $(SRC_TRACE) -o $@


.PHONY: clean

clean:
	rm *.out
