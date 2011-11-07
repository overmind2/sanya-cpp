# automatically generated from .pymakegen_v2.conf
# timestamp: 1320653797.403975
# handler_namespaces: ['root', handler.gcc]

gcc_CC=g++
gcc_CFLAGS=-ggdb3 -O3 -Wall -Winline -Wwrite-strings  \
	    -Wno-unused -c
gcc_INCLUDES=-I./
gcc_LDFLAGS=-O1 -ggdb3
gcc_TARGET=omscmv5-c

all : $(gcc_TARGET)
	
$(gcc_TARGET) : main.o heap.o objectmodel.o
	$(gcc_CC) $(gcc_LDFLAGS) main.o heap.o objectmodel.o -o  \
	    $(gcc_TARGET)

heap.o : heap.cpp heap-inl.hpp heap.hpp sanya.hpp  \
	    objectmodel.hpp handle.hpp sanya.hpp handle-inl.hpp  \
	    objectmodel-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) heap.cpp $(gcc_INCLUDES) -o heap.o

main.o : main.cpp heap-inl.hpp heap.hpp sanya.hpp  \
	    objectmodel.hpp handle.hpp sanya.hpp handle-inl.hpp  \
	    objectmodel-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) main.cpp $(gcc_INCLUDES) -o main.o

objectmodel.o : objectmodel.cpp objectmodel-inl.hpp  \
	    heap-inl.hpp objectmodel.hpp heap.hpp sanya.hpp handle.hpp  \
	    sanya.hpp handle-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) objectmodel.cpp $(gcc_INCLUDES) -o  \
	    objectmodel.o

clean : 
	rm -rf main.o heap.o objectmodel.o
.PHONY : clean
