_OBJ=Logic2BDD.o Transform.o synExp.o mytime.o SemanticsCudd.o bddBuilder.o humanNums.o eval.o cuddAdapter.o  constraintSorter.o minspanSorter.o RememberSorter.o MenuBlock.o maxOccSorter.o expSorter.o Scorer.o PermScorer.o SiftScorer.o RandomScorer.o forceScorer.o BlockScorer.o forceBlockScorer.o OtherScorer.o MultiComponents.o OneComponent.o compSorter.o DynamicScorer.o symbolTable.o configInfo.o choiceInfo.o varInfo.o 

OBJ = $(patsubst %,../obj/%,$(_OBJ))
DEP = $(OBJ:.o=.d)  # one dependency file for each source


INCLUDEFLAGS=-I/opt/local/include -I../tools/src -Isrc -I/usr/local/include -I../include

LDFLAGS=-lcudd -L/usr/local/lib -L/opt/local/lib -L../lib

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
       CXXFLAGS=-std=c++11 -O3 -g -I. -I/usr/local/include  
       LDFLAGS += -lpthread
    endif
    ifeq ($(UNAME_S),Darwin)
        CXXFLAGS=-fcolor-diagnostics -O3 -Wno-deprecated-register -std=c++11 -stdlib=libc++ -g -mtune=native
	LDFLAGS += -Wl,-stack_size,0x40000000,-stack_addr,0xf0000000
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64:wq
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

CC=c++

../bin/Logic2BDD: ../obj/synExpParser.o ../obj/synExpScanner.o ../obj/synExpDriver.o ${OBJ} 
	@echo \\t"[CXX]  " \\t$@
	@$(CC) $(CXXFLAGS) -o $@ $^ $(INCLUDEFLAGS) $(LDFLAGS)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif


# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
../obj/%.d: src/%.cpp
	@$(CPP) $(CFLAGS) $(INCLUDEFLAGS) $< -MM -MT $(@:.d=.o) >$@


.PHONY: clean
clean:
	-@rm -f ${OBJ} ${DEP}  ../bin/Logic2BDD src/synExpParser.hpp src/synExpScanner.cpp src/synExpParser.output src/synExpParser.cpp src/location.hh src/stack.hh src/position.hh ../obj/synExpDriver.o  ../obj/synExpParser.o  ../obj/synExpScanner.o

src/synExpParser.cpp: ../tools/src/synExpParser.ypp src/synExpScanner.cpp 
	@echo \\t[BISON] \\t$@ 
	@bison -Wno-conflicts-sr -v -o $@ -t -b $*  $< 

src/synExpScanner.cpp: ../tools/src/synExpScanner.lpp ../tools/src/synExpScanner.hpp
	@echo \\t"[FLEX] " \\t$@
	@flex --c++ -L  -o $@ $<

../obj/%.o: src/%.cpp src/%.hpp
	@echo \\t"[CXX]  " \\t$@
	@$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $<

../obj/%.o: ../tools/src/%.cpp ../tools/src/%.hpp
	@echo \\t"[CXX]  " \\t$@
	@$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $<

../obj/%.o: src/%.cpp
	@echo \\t"[CXX]  " \\t$@
	@$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $<
