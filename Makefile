CC = g++

all: test.out trace.out


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
test.out: 
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ $< $(SRC_TEST) -o $@
trace.out:
	$(CC) $(CFLAGS) $(LIBFLAGS) -L../ $< $(SRC_TRACE) -o $@


.PHONY: clean

clean:
	rm *.out
