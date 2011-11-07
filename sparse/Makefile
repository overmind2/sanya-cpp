# automatically generated from .pymakegen_v2.conf
# timestamp: 1320667214.687055
# handler_namespaces: ['root', handler.flexbison, handler.gcc]

flexbison_BISON=bison
flexbison_FLEX=flex
gcc_CC=g++
gcc_CFLAGS=-ggdb3 -O0 -Wall -Winline -Wwrite-strings  \
	    -Wno-unused -c
gcc_INCLUDES=-I./ -I../
gcc_LDFLAGS=-O1 -ggdb3
gcc_TARGET=sparse

all : scm_token.flex.h scm_token.flex.c scm_syntax.bison.h  \
	    scm_syntax.bison.c $(gcc_TARGET)
	
$(gcc_TARGET) : scm_token.flex.o scm_syntax.bison.o

scm_syntax.bison.c : scm_syntax.y
	$(flexbison_BISON) -o scm_syntax.bison.c scm_syntax.y

scm_syntax.bison.h : scm_syntax.y
	$(flexbison_BISON) -o /dev/null scm_syntax.y  \
	    --defines=scm_syntax.bison.h

scm_syntax.bison.o : scm_syntax.bison.c sanya_api.h  \
	    ../objectmodel-inl.hpp ../handle-inl.hpp ../objectmodel.hpp  \
	    ../handle.hpp ../sanya.hpp scm_token.flex.h
	$(gcc_CC) $(gcc_CFLAGS) scm_syntax.bison.c $(gcc_INCLUDES) -o  \
	    scm_syntax.bison.o

scm_token.flex.c : scm_token.lex
	$(flexbison_FLEX) -o scm_token.flex.c scm_token.lex

scm_token.flex.h : scm_token.lex
	$(flexbison_FLEX) --header-file=scm_token.flex.h -o /dev/null  \
	    scm_token.lex

scm_token.flex.o : scm_token.flex.c scm_syntax.bison.h  \
	    sanya_api.h ../objectmodel-inl.hpp ../handle-inl.hpp  \
	    ../objectmodel.hpp ../handle.hpp ../sanya.hpp
	$(gcc_CC) $(gcc_CFLAGS) scm_token.flex.c $(gcc_INCLUDES) -o  \
	    scm_token.flex.o

clean : 
	rm -rf scm_token.flex.h scm_token.flex.c scm_syntax.bison.h  \
	    scm_syntax.bison.c 
	 rm -rf scm_token.flex.o  \
	    scm_syntax.bison.o
.PHONY : clean
