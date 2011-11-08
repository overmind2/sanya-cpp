# automatically generated from .pymakegen_v2.conf
# timestamp: 1320682289.815093
# handler_namespaces: ['root', handler.flexbison, handler.gcc]

flexbison_BISON=bison
flexbison_FLEX=flex
gcc_CC=g++
gcc_CFLAGS=-ggdb3 -O0 -Wall -Winline -Wwrite-strings  \
	    -Wno-unused -c
gcc_INCLUDES=-I./
gcc_LDFLAGS=-O1 -ggdb3
gcc_TARGET=omscmv5-c

all : sparse/scm_token.flex.h sparse/scm_token.flex.c  \
	    sparse/scm_syntax.bison.h sparse/scm_syntax.bison.c  \
	    $(gcc_TARGET)
	
$(gcc_TARGET) : main.o heap.o objectmodel.o handlezone.o  \
	    sparse/scm_token.flex.o sparse/scm_syntax.bison.o
	$(gcc_CC) $(gcc_LDFLAGS) main.o heap.o objectmodel.o  \
	    handlezone.o sparse/scm_token.flex.o sparse/scm_syntax.bison.o  \
	    -o $(gcc_TARGET)

handlezone.o : handlezone.cpp handlezone.hpp
	$(gcc_CC) $(gcc_CFLAGS) handlezone.cpp $(gcc_INCLUDES) -o  \
	    handlezone.o

heap.o : heap.cpp heap-inl.hpp heap.hpp sanya.hpp  \
	    objectmodel.hpp handle.hpp sanya.hpp handle-inl.hpp  \
	    objectmodel-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) heap.cpp $(gcc_INCLUDES) -o heap.o

main.o : main.cpp handle-inl.hpp objectmodel-inl.hpp heap.hpp  \
	    sanya.hpp sparse/parse_api.h handle-inl.hpp sanya.hpp  \
	    objectmodel-inl.hpp objectmodel.hpp handle.hpp sanya.hpp  \
	    handle.hpp heap-inl.hpp heap-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) main.cpp $(gcc_INCLUDES) -o main.o

objectmodel.o : objectmodel.cpp objectmodel-inl.hpp  \
	    heap-inl.hpp objectmodel.hpp heap.hpp sanya.hpp handle.hpp  \
	    sanya.hpp handle-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) objectmodel.cpp $(gcc_INCLUDES) -o  \
	    objectmodel.o

sparse/scm_syntax.bison.c : sparse/scm_syntax.y
	$(flexbison_BISON) -o sparse/scm_syntax.bison.c  \
	    sparse/scm_syntax.y

sparse/scm_syntax.bison.h : sparse/scm_syntax.y
	$(flexbison_BISON) -o /dev/null sparse/scm_syntax.y  \
	    --defines=sparse/scm_syntax.bison.h

sparse/scm_syntax.bison.o : sparse/scm_syntax.bison.c  \
	    sparse/parse_api.h handle-inl.hpp sanya.hpp objectmodel-inl.hpp \
	     objectmodel.hpp heap.hpp sanya.hpp handle.hpp sanya.hpp  \
	    handle.hpp heap-inl.hpp sparse/sanya_api.h handle-inl.hpp  \
	    sanya.hpp objectmodel-inl.hpp handle.hpp heap-inl.hpp  \
	    sparse/scm_token.flex.h
	$(gcc_CC) $(gcc_CFLAGS) sparse/scm_syntax.bison.c  \
	    $(gcc_INCLUDES) -o sparse/scm_syntax.bison.o

sparse/scm_token.flex.c : sparse/scm_token.lex
	$(flexbison_FLEX) -o sparse/scm_token.flex.c  \
	    sparse/scm_token.lex

sparse/scm_token.flex.h : sparse/scm_token.lex
	$(flexbison_FLEX) --header-file=sparse/scm_token.flex.h -o  \
	    /dev/null sparse/scm_token.lex

sparse/scm_token.flex.o : sparse/scm_token.flex.c  \
	    sparse/scm_syntax.bison.h sparse/sanya_api.h handle-inl.hpp  \
	    sanya.hpp objectmodel-inl.hpp objectmodel.hpp heap.hpp  \
	    sanya.hpp handle.hpp sanya.hpp handle.hpp heap-inl.hpp
	$(gcc_CC) $(gcc_CFLAGS) sparse/scm_token.flex.c  \
	    $(gcc_INCLUDES) -o sparse/scm_token.flex.o

clean : 
	rm -rf sparse/scm_token.flex.h sparse/scm_token.flex.c  \
	    sparse/scm_syntax.bison.h sparse/scm_syntax.bison.c 
	 rm -rf  \
	    main.o heap.o objectmodel.o handlezone.o  \
	    sparse/scm_token.flex.o sparse/scm_syntax.bison.o
.PHONY : clean
