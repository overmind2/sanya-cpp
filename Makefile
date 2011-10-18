# automatically generated from .pymakegen_v2.conf
# timestamp: 1318965139.206126
# handler_namespaces: ['root', handler.gcc]

gcc_CC=g++
gcc_CFLAGS=-ggdb3 -O0 -Wall -Winline -Wwrite-strings  \
	    -Wno-unused -c
gcc_INCLUDES=-I./
gcc_LDFLAGS=-O0 -ggdb3
gcc_TARGET=omscmv5-c

all : $(gcc_TARGET)
	
$(gcc_TARGET) : main.o heap.o objectmodel.o
	$(gcc_CC) $(gcc_LDFLAGS) main.o heap.o objectmodel.o -o  \
	    $(gcc_TARGET)

heap.o : heap.cpp heap.hpp objectmodel.hpp heap.hpp
	$(gcc_CC) $(gcc_CFLAGS) heap.cpp $(gcc_INCLUDES) -o heap.o

main.o : main.cpp heap.hpp objectmodel.hpp heap.hpp
	$(gcc_CC) $(gcc_CFLAGS) main.cpp $(gcc_INCLUDES) -o main.o

objectmodel.o : objectmodel.cpp objectmodel.hpp heap.hpp
	$(gcc_CC) $(gcc_CFLAGS) objectmodel.cpp $(gcc_INCLUDES) -o  \
	    objectmodel.o

clean : 
	rm -rf main.o heap.o objectmodel.o
.PHONY : clean
